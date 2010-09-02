#include <sstream>
#include <iostream>
#include <iconv.h>
#include <cstring>
#include "OSCAR.h"

using namespace std;

unsigned char OSCAR::_u8(const char *data)
{
	return *data;
}

unsigned short OSCAR::_u16(const char *data)
{
	return (_u8(data) << 8) + _u8(data+1);
}

unsigned short OSCAR::_u16l(const char *data)
{
	return (_u8(data+1) << 8) + _u8(data);
}

unsigned int OSCAR::_u32(const char *data)
{
	return (_u16(data) << 16) + _u16(data+2);
}


unsigned int OSCAR::_u32l(const char *data)
{
	return (_u16l(data+2) << 16) + _u16l(data);
}

unsigned char OSCAR::u8()
{
	if(__data.length < 1)
		return 0;
	unsigned short tmp = _u8(__data.value);
	__data.length -= 1;
	__data.value += 1;
	return tmp;
}

unsigned short OSCAR::u16()
{
	if(__data.length < 2)
		return 0;
	unsigned short tmp = _u16(__data.value);
	__data.length -= 2;
	__data.value += 2;
	return tmp;
}

unsigned short OSCAR::u16l()
{
	if(__data.length < 2)
		return 0;
	unsigned short tmp = _u16l(__data.value);
	__data.length -= 2;
	__data.value += 2;
	return tmp;
}

unsigned int OSCAR::u32()
{
	if(__data.length < 4)
		return 0;
	unsigned short tmp = _u32(__data.value);
	__data.length -= 4;
	__data.value += 4;
	return tmp;
}


unsigned int OSCAR::u32l()
{
	if(__data.length < 4)
		return 0;
	unsigned short tmp = _u32l(__data.value);
	__data.length -= 4;
	__data.value += 4;
	return tmp;
}


std::string OSCAR::string(size_t len)
{
	if(len == 0 || __data.length < len)
		return "";
	std::string tmp;
	tmp.assign(__data.value, len);
	__data.value += len;
	__data.length -= len;
	return tmp;
}

std::string OSCAR::string08()
{
	return string(u8());
}

std::string OSCAR::string16()
{
	return string(u16());
}

OSCAR::OSCAR(data_t data) : Packet(data)
{
}


OSCAR::OSCAR() : Packet()
{
}

std::string OSCAR::conv_1251_utf8(std::string &in)
{
	iconv_t c = iconv_open("utf8", "cp1251");
	char *p_in = (char*)in.c_str();
	size_t in_len = in.length();
	char buffer[65535];
	memset(&buffer, 0, 65535);
	char *p_out = buffer;
	size_t out_len = 65535;
	size_t res = iconv(c, &p_in, &in_len, &p_out, &out_len);

	std::string result;
	if(res != -1)
		result.assign(buffer, 65535-out_len);
	return result;
}

std::string OSCAR::conv_ucs2_utf8(std::string &in)
{
	iconv_t c = iconv_open("utf8", "ucs2be");
	char *p_in = (char*)in.c_str();
	size_t in_len = in.length();
	char buffer[65535];
	memset(&buffer, 0, 65535);
	char *p_out = buffer;
	size_t out_len = 65535;
	size_t res = iconv(c, &p_in, &in_len, &p_out, &out_len);

	std::string result;
	if(res != -1)
		result.assign(buffer, 65535-out_len);
	else
	    std::cerr << "Error converting from UCS2BE to UTF8 data: " << in << std::endl;
	return result;
}

void OSCAR::parseF4S6C1(std::string &data)
{
	while(__data.length > 4) {
		unsigned short type = u16();
		unsigned short len = u16();
		switch(type) {
			case 2:
				while(len > 4) {
					size_t tmp = __data.length;
					unsigned char frag_id = u8();
					unsigned char frag_ver = u8();
					unsigned short frag_len = u16();
					if(frag_id == 1) {
						unsigned short charset = u16();
						unsigned short lang = u16();
						data = string(frag_len-4);
						if(charset)
							std::cerr << "Charset: " << charset << std::endl;
						if(lang)
							std::cerr << "Lang: " << lang << std::endl;
						if(charset == 2) {
							data = conv_ucs2_utf8(data);
						} else {
							data = conv_1251_utf8(data);
						}
						return;
					} else {
						string(frag_len);
					}
					len -= tmp - __data.length;
				}
				break;
			default:
				string(len);
				break;
		}
	}
}

void OSCAR::dump(size_t length)
{
	std::cerr << std::hex;
	for(int i=0; i<length; ++i) {
		if(i && i%16 == 0)
			std::cerr << std::endl;
		std::cerr << (unsigned int)u8() << " ";
	}
	std::cerr << std::endl;
}

void OSCAR::dump(std::string & in)
{
	std::cerr << std::hex;
	for(int i=0; i<in.length(); ++i) {
		if(i && i%16 == 0)
			std::cerr << std::endl;
		std::cerr << (unsigned int)in[i] << " ";
	}
	std::cerr << std::endl;
}

int spc_utf8_isvalid(const unsigned char *input) {
  int nb;
  const unsigned char *c = input;

  for (c = input;  *c;  c += (nb + 1)) {
    if (!(*c & 0x80)) nb = 0;
    else if ((*c & 0xc0) == 0x80) return 0;
    else if ((*c & 0xe0) == 0xc0) nb = 1;
    else if ((*c & 0xf0) == 0xe0) nb = 2;
    else if ((*c & 0xf8) == 0xf0) nb = 3;
    else if ((*c & 0xfc) == 0xf8) nb = 4;
    else if ((*c & 0xfe) == 0xfc) nb = 5;
    while (nb-- > 0)
      if ((*(c + nb) & 0xc0) != 0x80) return 0;
  }
  return 1;
}

bool OSCAR::utf8_valid(std::string &data)
{
/*    size_t i = 0;
    unsigned char byteno = 0;
    while(i < data.length()) {
	if(! (data[i] & 0x80)) byteno = 0;
	else if ((data[i] & 0xc0) == 0x80) return false;
	else if ((data[i] & 0xe0) == 0xc0) byteno = 1;
	else if ((data[i] & 0xf0) == 0xe0) byteno = 2;
	else if ((data[i] & 0xf8) == 0xf0) byteno = 3;
	else if ((data[i] & 0xfc) == 0xf8) byteno = 4;
	else if ((data[i] & 0xfe) == 0xfc) byteno = 5;
	if(byteno + i >= data.length())
	    return true;
	while(byteno -- > 0)
	    if((data[i+byteno] & 0xc0) != 0x80)
		return false;
	
	i++;
    }
    return true;
    */
    
    return spc_utf8_isvalid((const unsigned char*)data.c_str()) == 1;
}

void OSCAR::parseF4S6C2(std::string &data)
{
	while(__data.length > 4) {
		unsigned short type = u16();
		unsigned short len = u16();
		size_t sav = __data.length;
#ifdef DEBUG
		std::cerr << "Readed TLV " << type << ", length " << len << std::endl;
#endif
		switch(type) {
			case 5:
				std::cerr << "Message type: " << u16() << std::endl; //Message type
				string(8); // Message cookie
				string(16); // Capability
				while(__data.length && sav - __data.length < len) {
#ifdef DEBUG
					std::cerr << "Sav: " << sav << ", clen:" << __data.length << std::endl;
#endif
					unsigned short t1 = u16();
					unsigned short l1 = u16();
					data_t sav1 = __data;
#ifdef DEBUG
					std::cerr << "SUB TLV: " << t1 << ", len=" << l1 << std::endl;
#endif
					if(t1 == 0x2711) {
						unsigned short l2 = u16l();
#ifdef DEBUG
						std::cerr << "Capability-specific length: " << l2 << std::endl;
#endif
						if(l2 == 27) {
							u16(); //protocol version
							bool is_plugin = false;
							for(int i=0; i<16; ++i)
							    if(u8())
								is_plugin = true;
							string(9);
							
							if(is_plugin) { // read rest of data
							    std::cerr << "It's is a plugin message" << std::endl;
							    string(l1-l2);
							} else {
							    std::cerr << "It's is a generic message" << std::endl;
							    unsigned short l3 = u16l();
							    string(l3); // skip unneeded data
							    
							    unsigned char m_type = u8();
							    unsigned char m_flags = u8();
							    std::cerr << "MSG TYPE: " << (int)m_type << ", FLAGS: " << (int)m_flags << std::endl;
							    u16();//status code
							    u16();//priority
							    l3 = u16l();
							    data = string(l3);
							    bool is_utf8 = false;
							    if(m_type = 1) {
								u32(); // foreground;
								u32(); // background;
								unsigned int guid_len = u32l();
								if(guid_len) {
								    if(string(guid_len) == "{0946134E-4C7F-11D1-8222-444553540000}") {
									is_utf8 = true;
								    }
								}
							    }
							    dump(data);
							    if(!utf8_valid(data)) {
								if(is_utf8) {
								    data = conv_ucs2_utf8(data);
								} else {
								    data = conv_1251_utf8(data);
								}
							    }
							}
						} else {
						    string(l1-2);
						}
					} else {
					    string(l1);
					}

					__data = sav1;
					string(l1);
				}
				if(sav - __data.length < len)
					string(sav - __data.length - len);
				break;

			default:
				if(len>0)
#ifdef DEBUG
					dump(len);
#else
					string(len);
#endif
				break;
		}
	}
}

void OSCAR::parseF4S6()
{
	// Family 4 subtype 6 snac - message to server
	u32(); // skip message cookie
	u32(); // 8 bytes
	unsigned short chan = u16();
	std::string uin = string08();
	std::string msg;
#ifdef DEBUG
	std::cerr << "UIN: " << uin << ", Channel: " << chan << std::endl;
#endif
	switch(chan) {
		case 1:
			parseF4S6C1(msg);
			break;
		case 2:
			parseF4S6C2(msg);
			break;
		default:
			std::cerr << "Unknown message channel" << std::endl;
			break;
	}
	if(msg.length()) {
		std::cerr << "Sending message to UIN " << uin << "at chan" << chan << ": " << msg << std::endl;
		_data.push_back(msg);
	}
}

void OSCAR::tlvBlock()
{
    unsigned short cnt = u16();
    for(unsigned short i=0; i<cnt; ++i) {
	u16(); // tag
	string16(); // skip
    }
}

void OSCAR::parseF4S7()
{
	// Family 4 subtype 6 snac - message to server
	string(8); // skip message cookie
	unsigned short chan = u16();
	std::string uin = string08();
	u16(); // Sender warning level
	tlvBlock(); // Number of TLVs in fixed part skip
	
	std::string msg;
	switch(chan) {
		case 1:
			parseF4S6C1(msg);
			break;
		case 2:
			parseF4S6C2(msg);
			break;
		default:
			std::cerr << "Unknown message channel" << std::endl;
			break;
	}
	if(msg.length()) {
		std::cerr << "Received message from UIN " << uin << ": " << msg << std::endl;
		_data.push_back(msg);
	}
}

void OSCAR::parseF4()
{
	// Family 4 snac - messaging service
	switch(subtype) {
		case 6: // send message
			parseF4S6();
			break;
		case 7: // recv message
			parseF4S7();
			break;
		default:
			break;
	}
}

void OSCAR::parseSNAC()
{
	family = u16();
	subtype = u16();
	flags = u16();
	reqid = u32();
#ifdef DEBUG
	std::cerr << "SNAC" << std::endl
		<< "\t" << "Service:  0x" << family << std::endl
		<< "\t" << "Subtype:  0x" << subtype << std::endl
		<< "\t" << "Flags:    0x" << flags << std::endl
		<< "\t" << "RequestID:0x" << reqid << std::endl << std::dec;
#endif

	switch(family) {
		case 4: // ICBM - messages
			parseF4();
			break;
		default:
			break;
	}
}

bool OSCAR::parse1()
{
}

bool OSCAR::parse2()
{
	parseSNAC();
	return true;
}

bool OSCAR::parse3()
{
}

bool OSCAR::parse4()
{
}

bool OSCAR::parse5()
{
}

bool OSCAR::parseFLAP()
{
	if(__data.length < 6) // FLAP header is 6 bytes, therefore it's a minimum for OSCAR
		return false;
	
	if(u8() != 0x2a) // FLAP mark is invalid
		return false;

	unsigned char type = u8();
	unsigned short seqnum = u16();
	length = u16();
#ifdef DEBUG
	std::cerr << "Flap " << (int)type << " seq " << seqnum << ", len " << length << std::endl;
#endif
	switch(type) { // Type of the flap
		case 1: // Sign on flap
			parse1();
			break;
		case 2: // DATA flap (most popular)
			parse2();
			break;
		case 3: // Error flap (very rare)
			parse3();
			break;
		case 4: // Sign off flap
			parse4();
			break;
		case 5: // keepalive flap
			parse5();
			break;
		default:
			return false;
	}
}

void OSCAR::parse()
{
	parseFLAP();
}

Packet * OSCAR::nextProto()
{
	return NULL;
}

std::string OSCAR::pack()
{
	std::string tmp = "\t\nOSCAR Session\n";
	return tmp;
}

void OSCAR::output(std::ostream &os)
{
    os << '\0' << "OSCAR SESSION";
    if(!_data.size())
	return;
    size_t icnt = 0, j;
    list<std::string>::iterator i, iend = _data.end();
    for(i=_data.begin() ; i != iend; ++i) {
	os << '\0' << "LINE:" << (icnt++)
	   << '\0';
	size_t len = i->length();
	for(j = 0; j < len ; ++j) {
	    char k = (*i)[j];
	    switch(k) {
		case 0:
		    os << "\0";
		    break;
		case '\\':
		    os << "\\\\";
		    break;
		case '\n':
		    os << "\\n";
		    break;
		default:
		    os << k;
		    break;
	    };
	}
    }
}

