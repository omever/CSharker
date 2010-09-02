#include "TCP.h"
#include "HTTP.h"
#include "FTP.h"
#include "OSCAR.h"

TCP::TCP(data_t indata) : IP(indata)
{
}

unsigned short TCP::src_port()
{
	return xshort(0);
}

unsigned short TCP::dst_port()
{
	return xshort(2);
}

unsigned int TCP::seq()
{
	return xint(4);
}

unsigned int TCP::ack()
{
	return xint(8);
}

size_t TCP::header_length()
{
	return ((xchar(12)&0xf0) >> 4) * 4 ;
}

bool TCP::is_urg()
{
	return xchar(13)&32;
}

bool TCP::is_ack()
{
	return xchar(13)&16;
}

bool TCP::is_push()
{
	return xchar(13)&8;
}

bool TCP::is_rst()
{
	return xchar(13)&4;
}

bool TCP::is_syn()
{
	return xchar(13)&2;
}

bool TCP::is_fin()
{
	return xchar(13)&1;
}

unsigned short TCP::window()
{
	return xshort(14);
}

unsigned short TCP::crc()
{
	return xshort(16);
}

unsigned short TCP::urgent()
{
	return xshort(18);
}

bool TCP::closeConnection()
{
	return is_fin() || is_rst();
}

Packet * TCP::nextProto()
{
    if(src_port() == 80 || dst_port() == 80)
	return new HTTP(this->next());

    if(src_port() == 21 || dst_port() == 21)
	return new FTP(this->next());

    if(src_port() == 5190 || dst_port() == 5190 || src_port() == 443 || dst_port() == 443)
	return new OSCAR(this->next());
	
    return NULL;
}

