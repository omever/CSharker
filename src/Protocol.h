#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <cstdlib>

class Connection;

struct data_t
{
	const char *value;
	size_t length;
};

class Protocol
{
public:
	virtual void data(data_t data) = 0;
	virtual void data(Protocol &p) = 0;
	virtual const data_t & data() = 0;
	virtual data_t next() = 0;
	virtual inline size_t header_length() = 0;
	virtual inline void parse() = 0;
	virtual Protocol * nextProto() = 0;
	virtual bool closeConnection() = 0;
	virtual std::string pack() = 0;

	virtual inline bool is_trackable() = 0;
	virtual inline void make_track(Connection &c) = 0;
	virtual void output(std::ostream &os) = 0;
};

#endif

