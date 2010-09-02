#ifndef TCP_H
#define TCP_H

#include "Packet.h"
#include "IP.h"
#include "connection.h"

class TCP : public IP
{
public:
	TCP() {};
	TCP(data_t data);

	unsigned short src_port();
	unsigned short dst_port();
	unsigned int seq();
	unsigned int ack();
	size_t header_length();
	bool is_urg();
	bool is_ack();
	bool is_push();
	bool is_rst();
	bool is_syn();
	bool is_fin();
	unsigned short window();
	unsigned short crc();
	unsigned short urgent();
	
	virtual Packet * nextProto();
	virtual inline bool is_trackable() { return true; };

	virtual inline void make_track(Connection &c) { 
		c.src_port = src_port(); 
		c.dst_port = dst_port(); 
		if(is_rst() || is_fin())
			c.close_wait = true;
	};
	virtual bool closeConnection();
};

#endif

