#include <string>
#include "Ethernet.h"
#include "IP.h"

using namespace std;

Ethernet::Ethernet(data_t idata) : Packet(idata)
{
}

string Ethernet::dst_mac()
{
	return string().assign(__data.value, 6);
}

string Ethernet::src_mac()
{
	return string().assign(__data.value+6, 6);
}

unsigned short Ethernet::type()
{
	return xshort(12);
}

Packet * Ethernet::nextProto()
{
	if(type() == 0x0800) {
		return new IP(next());
	}
	return NULL;
}
