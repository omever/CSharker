#include <sstream>
#include <iostream>
#include "IP.h"
#include "TCP.h"
#include "Ethernet.h"

/*
#define xchar(x, y) ((unsigned char)*(x + y))
#define xshort(x, y) ((xchar(x, y) << 8) + xchar(x, y+1))
#define xint(x, y) ((xshort(x, y) << 16) + xshort(x, y+2))
*/

using namespace std;

IP::IP(data_t indata) : Ethernet(indata)
{
    size_cnt = 0;
}

unsigned char IP::version()
{
	return (xchar(0)&0x240)>>4;
}

size_t IP::header_length()
{
	return (xchar(0)&0x15) * 4;
}

unsigned char IP::tos()
{
	return xchar(1);
}

unsigned short IP::length()
{
	return xshort(2);
}

unsigned short IP::id()
{
	return xshort(4);
}

bool IP::df_set()
{
	return xchar(6)&64 != 0;
}

bool IP::mf_set()
{
	return xchar(6)&32 != 0;
}

unsigned short IP::fragment_offset()
{
	return ((xchar(6)&31) << 8 + xchar(7));
}

unsigned char IP::ttl()
{
	return xchar(8);
}

unsigned char IP::proto()
{
	return xchar(9);
}

unsigned short IP::crc()
{
	return xshort(10);
}

unsigned int IP::src()
{
	return xint(12);
}

unsigned int IP::dst()
{
	return xint(16);
}

string IP::i2a(unsigned int ip)
{
	stringstream tmp;
	tmp << ((ip >> 24)&0xff)
		<< '.'
		<< ((ip >> 16)&0xff)
		<< '.'
		<< ((ip >> 8) &0xff)
		<< '.'
		<< (ip & 0xff);
	return tmp.str();
}

void IP::parse()
{
	last_seen = time(NULL);
	size_cnt += length();
}

Packet * IP::nextProto()
{
	if(proto() == 6) {
		return new TCP(next());
	}
	return NULL;
}

bool IP::closeConnection()
{ // 5 minutes IP conntrack timeout
	return time(NULL) - last_seen > 300;
}

void IP::output(std::ostream &os)
{
    os << '\0' << "IP TRACK" << '\0' << "SIZE:" << size_cnt;
}
