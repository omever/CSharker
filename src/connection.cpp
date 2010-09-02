#include <sstream>
#include "connection.h"
#include "IP.h"

using namespace std;

ostream &operator<<(ostream &os, const Connection &conn) {
    os << conn.start << '\0' 
       << conn.ts << '\0' 
       << IP::i2a(conn.saddr) << '\0' 
       << IP::i2a(conn.daddr) << '\0'
       << conn.bytes << '\0'
       << conn.proto << '\0'
       << conn.src_port << '\0'
       << conn.dst_port << '\0';
    if(conn.conn_data != NULL) {
	conn.conn_data->output(os);
    }
}

Connection::Connection() 
{
	clear();
}

Connection::~Connection()
{
	if(conn_data != NULL)
		delete conn_data;
}

std::string Connection::string() const
{
	std::stringstream tmp;
	tmp << saddr << "->" << daddr << " " << proto << ": " << src_port << "->" << dst_port;
	return tmp.str();
}

bool Connection::operator== (const Connection &conn) const
{
	return (!(conn.sent || sent)) &&(proto == conn.proto) && (
		(saddr == conn.saddr) 
		&& (daddr == conn.daddr)
		&& (src_port == conn.src_port)
		&& (dst_port == conn.dst_port)
	) || (
		(saddr == conn.daddr)
		&& (daddr == conn.saddr)
		&& (src_port == conn.dst_port)
		&& (dst_port == conn.src_port)
	);
}


