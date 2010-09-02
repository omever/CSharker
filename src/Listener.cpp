#include <cstdio>
#include <pcap.h>
#include <iostream>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <typeinfo>

#include "Ethernet.h"
#include "IP.h"
#include "TCP.h"
#include "HTTP.h"
#include "Listener.h"
#include "conntrack.h"
#include "main.h"
#include "exporter.h"

using namespace std;

Listener::Listener(const char *device)
{
	_bill.connect("BILLING", "DBA", "123");

	char *dev, errbuf[PCAP_ERRBUF_SIZE];
	if(device != NULL) {
		dev = (char*)device;
	} else {
		dev = pcap_lookupdev(errbuf);
	}

	if(dev == NULL) {
		cerr << "Could not find default device: " << errbuf << endl;
	}

	cerr << "Listening on the " << dev << " device" << endl;

	char filter[] = "port 80";
	struct bpf_program fp;

	h = pcap_open_live(dev, 65535, 1, 0, errbuf);

	if(pcap_compile(h, &fp, filter, 0, 0) == -1) {
		cerr << "Could not parse filter " << pcap_geterr(h) << endl;
	} else {
		if(pcap_setfilter(h, &fp) == -1) {
			cerr << "Could not install filter: " << pcap_geterr(h) << endl;
		}
	}
}

Listener::~Listener()
{
	pcap_close(h);
}

unsigned short chksum(void * data, unsigned short len)
{
	unsigned long sum = 0;
	unsigned short *temp = (unsigned short*)data;
	while(len > 1) {
		sum += *temp++;
		if(sum >> 16)
			sum = (sum &0xffff) + (sum >> 16);
		len -= 2;
	}

	if(len)
		sum += (unsigned short) *((unsigned char *)temp);

	if(sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~((unsigned short)sum);
}

unsigned short ip_chksum(void *data)
{
	struct iphdr *hdr = (struct iphdr*)data;
	return chksum(data, hdr->ihl*4);
}

unsigned short tcp_chksum(void *data)
{
	struct pseudo_header {
		unsigned long saddr;
		unsigned long daddr;
		unsigned char zero;
		unsigned char proto;
		unsigned short len;
	} *pheader;
	unsigned short sum = 0;
	struct iphdr *hdr = (struct iphdr*)data;
	struct tcphdr * tcp = (struct tcphdr*)((unsigned char*)data+hdr->ihl * 4);

	unsigned short llen = ntohs(hdr->tot_len) - hdr->ihl*4;
	cerr << "Len: " << sizeof(struct pseudo_header) << ", " << llen << endl;
    	unsigned char * tmpdata = new unsigned char[sizeof(struct pseudo_header) + llen];
	
	pheader = (struct pseudo_header *) tmpdata;
	pheader->saddr = hdr->saddr;
	pheader->daddr = hdr->daddr;
	pheader->zero = 0;
	pheader->proto = hdr->protocol;
	pheader->len = htons(llen);
	memcpy(tmpdata + sizeof(struct pseudo_header), (void*)tcp, llen);
	sum = chksum(tmpdata, llen+sizeof(struct pseudo_header));
	delete [] tmpdata;

	return sum;
}

void terminateRequest(IP &i, TCP &t)
{
    int sd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
	if(sd <= 0) {
		cerr << "Error opening raw socket" << endl;
		return;
	}
    int sm = 1;
    int res = setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &sm, sizeof(sm));
    if(res == -1) {
		cerr << "Error setting raw ip option" << endl;
		return;
	}
	size_t tlen = sizeof(struct iphdr) + sizeof(struct tcphdr);
    unsigned char pkt[tlen];
    memset(pkt, 0, tlen);
    struct iphdr * hdr = (struct iphdr*)pkt;
    hdr->ihl = 5;
    hdr->version = IPVERSION;
    hdr->tos = 0;
    hdr->tot_len = htons(tlen);
    hdr->id = 0;
    hdr->frag_off = 0;
    hdr->ttl = IPDEFTTL;
    hdr->protocol = IPPROTO_TCP;
	hdr->saddr = inet_addr(i.i2a(i.dst()).c_str());
	hdr->daddr = inet_addr(i.i2a(i.src()).c_str());
	hdr->check = ip_chksum(pkt);
    
    struct tcphdr * tcp = (struct tcphdr*)(pkt+hdr->ihl * 4);
    tcp->source = htons(t.dst());
    tcp->dest = htons(t.src());
    tcp->seq = 0; //t.ack();
    tcp->ack_seq = 0;
    tcp->res1 = 0;
    tcp->doff = sizeof(struct tcphdr) >> 2;
    tcp->rst = 1;
	tcp->fin = 0;
	tcp->ack = 0;
    tcp->window = 0;
    tcp->check = 0;
    tcp->urg_ptr = 0;
   
	unsigned short len = tlen - hdr->ihl*4;

	tcp->check = tcp_chksum(pkt);
		//csum_tcpudp_magic(hdr->saddr, hdr->daddr, len, IPPROTO_TCP, 0);

	struct sockaddr_in sout;
	sout.sin_family = AF_INET;
	sout.sin_addr.s_addr = hdr->daddr;
	sout.sin_port = hdr->protocol;

	cerr << "tlen: " << tlen << endl;
    res = sendto(sd, pkt, tlen, 0, (struct sockaddr*)&sout, sizeof(sout));
    close(sd);
	if(res == -1) {
		cerr << "Error sending RST packet " << strerror(errno) << endl;
	}
}

void Listener::start()
{
	pcap_pkthdr header;
	const u_char * data;
	DQueue queue;
	ConnectionTracker connlist(queue);
	Exporter exporter(_bill, queue);
	Protocol *cur, *next;
	Connection c, *cnew;
	while(1)
	while((data = pcap_next(h, &header)) != NULL) {
		cur = new Packet();
		c.clear();
		c.ts = time(NULL);

		data_t d;
		d.value = (const char*)data;
		d.length = header.len;
		cur->data(d);
		while(1) {
			if(cur->is_trackable())
				cur->make_track(c);

			next = cur->nextProto();
			if(next == NULL)
				break;

			delete cur;
			cur = next;
		}

		cnew = connlist.lookup(c);
		if(cnew == NULL) {
			cnew = connlist.add(c);
		} else {
			cnew->merge(c);
		}
		
		cnew->bytes += header.len;
		
		if(cnew->conn_data == NULL) {
			cnew->conn_data = cur;
		} else {
			cnew->conn_data->data(*cur);
			delete cur;
		}
		
		cnew->conn_data->parse();
	}
}
