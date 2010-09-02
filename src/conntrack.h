#ifndef CONNTRACK_H
#define CONNTRACK_H

//#include <unordered_map>
#include <map>
#include <list>
#include <utility>
#include <string>
#include "Packet.h"
#include "connection.h"
#include "main.h"

class ConnectionTracker
{
public:
	ConnectionTracker(DQueue &);
	~ConnectionTracker();
	Connection * add(Connection &);
	void close(Connection &);
	Connection * lookup(Connection &);
	void lock();
	void unlock();
// do not use this directly!
	void operator() (Connection *);
protected:
	void internal_close(Connection &);
	void internal_close(unsigned long l, Connection &);

	static void * main_cycle(void*);
	void warehouse();
	void cleanup();

private:
	std::map<unsigned long, std::list<Connection> > _connmap;
	pthread_mutex_t _mutex;
	pthread_t _thread;
	int to_exit;
	DQueue *_queue;
};

#endif

