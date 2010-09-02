#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <cstdlib>
#include <iostream>
#include "Protocol.h"

class Packet;

class Packet : public Protocol
{
public:
	Packet() {};
	Packet(data_t data);
	virtual ~Packet();
	void data(data_t data);
	void data(Protocol &p);
	const data_t & data() {
		return __data;
	}
	virtual data_t next();
	virtual inline size_t header_length();
	virtual inline void parse() {};
	virtual Protocol * nextProto();
	virtual bool closeConnection();
	virtual std::string pack();

	virtual inline bool is_trackable() { return false; }
	virtual inline void make_track(Connection &c) { };
	virtual void output(std::ostream &os);
protected:
	inline unsigned char xchar(unsigned short off) {
		return (unsigned char)*(__data.value + off);
	}
	inline unsigned short xshort(unsigned short off) {
		return (xchar(off) << 8) + xchar(off+1);
	}
	inline unsigned int xint(unsigned short off) {
		return (xshort(off) << 16) + xshort(off+2);
	}
	
protected:
	data_t __data;
private:
};

#endif

