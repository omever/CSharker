#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <list>
#include "Packet.h"
#include "TCP.h"

enum HTTP_Type
{
	HTTP_REQUEST,
	HTTP_RESPONSE,
	NOT_HTTP
};

class HTTP : public TCP
{
public:
	HTTP(data_t);
	HTTP();
	virtual void parse();
	void clear();

	size_t header_length() { return __data.length; };
	virtual Packet* nextProto();
	virtual std::string pack();
	virtual void output(std::ostream &os);
private:
	std::list<std::string> _indata;
	std::string _last;
};
#endif

