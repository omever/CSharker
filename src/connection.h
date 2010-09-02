#ifndef CONNECTION_H
#define CONNECTION_H

#include <map>
#include <list>
#include <utility>
#include <string>
#include <iostream>

#include "Packet.h"

class Connection;

std::ostream &operator<<(std::ostream &os, const Connection &conn);

class Connection
{
public:
    Connection();
    ~Connection();
    unsigned long long bytes;
    unsigned long saddr;
    unsigned long daddr;
    unsigned short src_port;
    unsigned short dst_port;
    unsigned char proto;
    bool close_wait;
    bool exported;
    bool sent;
    time_t start, ts;
    Protocol *conn_data;

    void setExported(bool value) {
	exported = value;
    }
    bool operator== (const Connection &conn) const;
    std::string string() const;
	inline void clear() {
	    conn_data = NULL;
	    bytes = 0;
	    saddr = 0;
	    daddr = 0;
	    proto = 0;
	    src_port = 0;
	    dst_port = 0;
	    close_wait = false;
	    sent = false;
	    exported = false;
	    ts = 0;
	    start = time(NULL);
	};
	inline void merge(const Connection &conn)
	{
		close_wait = close_wait || conn.close_wait;
		ts = conn.ts;
	}
	
	friend std::ostream &operator<<(std::ostream &os, const Connection &conn);
	
private:
};

#endif

