#ifndef ETHERNET_H
#define ETHERNET_H

#include <string>
#include <pcap.h>
#include "Packet.h"

class Ethernet : public Packet
{
public:
	Ethernet() {};
	Ethernet(data_t data);
	std::string dst_mac();
	std::string src_mac();
	unsigned short type();
	size_t header_length() { return 14; };

	virtual Packet * nextProto();
};
#endif

