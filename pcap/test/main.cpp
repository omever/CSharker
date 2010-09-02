#include <cstdio>
#include <pcap.h>
#include <iostream>

using namespace std;

int main(void)
{
	char *dev, errbuf[PCAP_ERRBUF_SIZE];

	dev = pcap_lookupdev(errbuf);
	if(dev == NULL) {
		cerr << "Could not find default device: " << errbuf << endl;
		return 2;
	}

	cout << "Device: " << dev << endl;

	pcap_t *h;

	h = pcap_open_live(dev, 65535, 1, 0, errbuf);
	pcap_pkthdr header;
	const u_char * data;
	while(data = pcap_next(h, &header)) {
		cout << "Readed packet of " << header.len << endl;
	}
	pcap_close(h);
	return 0;
}
