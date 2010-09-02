#include "Packet.h"
#include "Ethernet.h"

Packet::Packet(data_t indata)
{
	data(indata);
}

Packet::~Packet()
{
	__data.length = 0;
	__data.value = 0;
}

void Packet::data(data_t indata)
{
	__data = indata;
}

void Packet::data(Protocol &indata)
{
	__data = indata.data();
}

data_t Packet::next()
{
	data_t tmp = __data;
	size_t len = header_length();
	if(len > tmp.length)
	    len = tmp.length;
	tmp.length -= len;
	tmp.value += len;
	return tmp;
}

size_t Packet::header_length()
{
    return __data.length;
}

Protocol * Packet::nextProto()
{
    return new Ethernet(__data);
}

bool Packet::closeConnection()
{
	return false;
}

std::string Packet::pack()
{
	return "";
}

void Packet::output(std::ostream &os)
{
    os << '\0' << "RAW PACKET TRACK";
}
