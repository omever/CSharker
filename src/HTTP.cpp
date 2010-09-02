#include <sstream>
#include <iostream>
#include "HTTP.h"

using namespace std;

HTTP::HTTP(data_t data) : TCP(data)
{
    _indata.clear();
}


HTTP::HTTP() : TCP()
{
    _indata.clear();
}

std::string chomp(std::string instr)
{
	size_t s1 = instr.find_first_not_of("\r\n");
	size_t s2 = instr.find_last_not_of("\r\n");
	return instr.substr(s1, s2-s1+1);
}

void HTTP::parse()
{
	string body;
	body.assign(__data.value, __data.length);
	size_t pos = body.find_first_of('\n');
	size_t lend = pos;
	size_t to = string::npos;

	HTTP_Type _type;

	if(pos == string::npos) {
		_type = NOT_HTTP;
		return;
	}
	
	string start = body.substr(0, pos);
	_last = start;
	if(start.substr(0, 5) == "HTTP/") {
		_type = HTTP_RESPONSE;
		_indata.push_back("HTTP RESPONSE");

		return;
	} else {
		pos = start.find_first_of(' ');
		if(pos == string::npos) {
			_type = NOT_HTTP;
			return;
		}

		std::string __request_method = start.substr(0, pos);
		if(!(__request_method == "GET" ||
				__request_method == "POST" ||
				__request_method == "HEADER" ||
				__request_method == "PUT" ||
				__request_method == "CONNECT")) {
			_type = NOT_HTTP;
			return;
		}
		pos = start.find_first_not_of(' ', pos);
		if(pos == string::npos) {
			_type = NOT_HTTP;
			return;
		}

		to = start.find_first_of(' ', pos);
		if(to == string::npos) {
			_type = NOT_HTTP;
			return;
		}
		std::string __request_uri = start.substr(pos, to-pos);

		pos = start.find_first_not_of(' ', to);
		if(to == string::npos) {
			_type = NOT_HTTP;
			return;
		}

		std::string __request_version = start.substr(pos);
		std::string __request_host = "";
		_type = HTTP_REQUEST;
		while((pos = body.find_first_of('\n', lend+1)) != string::npos) {
		    to = body.find_first_of(':', lend+1);
		    if(to == string::npos) {
			    lend = pos+1;
			    continue;
		    }
		    if(body.substr(lend+1, to-lend-1) == "Host") {
				__request_host = chomp(body.substr(to+2, pos - to - 1));
				break;
			}
		    lend = pos;
		}

		_indata.push_back(string("HTTP REQUEST: ") + __request_method + " http://" + __request_host + __request_uri + " " + __request_version);
	}

}

Packet * HTTP::nextProto()
{
	return NULL;
}

string HTTP::pack()
{
	string tmp = "\t\nHTTP Session\n";
	std::list<std::string>::iterator i, iend = _indata.end();
	for(i=_indata.begin() ; i != iend; ++i)
		tmp += "\t\t" + *i + "\n";
	return tmp;
}

void HTTP::output(std::ostream &os)
{
    os << '\0' << "HTTP SESSION";
    os << '\0';
    list<string>::iterator i, iend = _indata.end();
    //for(i=_indata.begin() ; i != iend; ++i) {
/*	size_t len = i->length();
	for(int j = 0; j < len ; ++j) {
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
	}*/
  //  }
}
