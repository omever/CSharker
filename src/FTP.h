#ifndef FTP_H
#define FTP_H

#include <string>
#include <list>
#include "Packet.h"
#include "TCP.h"

class FTP : public TCP
{
public:
	FTP(data_t);
	FTP();
	virtual void parse();
	void clear();

	virtual Packet* nextProto();
	virtual std::string pack();
private:
	std::list<std::string> __line;
};
#endif

