#include <sstream>
#include <iostream>
#include "FTP.h"

using namespace std;

FTP::FTP(data_t data) : TCP(data)
{
    __line.clear();
}


FTP::FTP() : TCP()
{
    __line.clear();
}

void FTP::parse()
{
	string body;
	body.assign(__data.value, __data.length);
	size_t last = 0;
	size_t pos;
	while((pos = body.find_first_of('\n', last)) != string::npos) {
	    __line.push_back(body.substr(last, pos-last-1));
	    last = pos + 1;
	};
	__line.push_back(body.substr(last));
}

Packet * FTP::nextProto()
{
	return NULL;
}

std::string FTP::pack()
{
	string tmp = "\t\nFTP Session\n";
	list<string>::iterator i, iend = __line.end();
	for(i = __line.begin(); i!=iend; ++i)
		tmp += "\t\t" + *i + "\n";
	return tmp;
}
