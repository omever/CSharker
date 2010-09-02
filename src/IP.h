#ifndef IP_H
#define IP_H

#include <string>
#include "Packet.h"
#include "Ethernet.h"
#include "connection.h"

class IP : public Ethernet
{
public:
	IP() {};
	IP(data_t data);
	virtual ~IP() {};
	unsigned char version();
	unsigned char tos();
	size_t header_length();
	unsigned short length();
	unsigned short id();
	bool df_set();
	bool mf_set();
	unsigned short fragment_offset();
	unsigned char ttl();
	unsigned char proto();
	unsigned short crc();
	unsigned int src();
	unsigned int dst();
	static std::string i2a(unsigned int);

	virtual void parse();
	virtual bool closeConnection();
	virtual Packet * nextProto();
	virtual inline bool is_trackable() { return true; };
	virtual inline void make_track(Connection &c) { c.saddr = src(); c.daddr = dst(); c.proto = proto(); };
	virtual void output(std::ostream &os);
private:
	time_t last_seen;
	long long size_cnt;
};
#endif

