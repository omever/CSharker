#include <algorithm>
#include <sstream>
#include <iostream>
#include "conntrack.h"

using namespace std;

ConnectionTracker::ConnectionTracker(DQueue &inqueue)
{
	_queue = &inqueue;
	pthread_mutex_init(&_mutex, NULL);
	to_exit = 0;
	pthread_create(&_thread, NULL, &main_cycle, this);
}

ConnectionTracker::~ConnectionTracker()
{
	to_exit = 1;
	pthread_join(_thread, NULL);
	pthread_mutex_destroy(&_mutex);
}

void ConnectionTracker::lock()
{
	pthread_mutex_lock(&_mutex);
}

void ConnectionTracker::unlock()
{
	pthread_mutex_unlock(&_mutex);
}

Connection* ConnectionTracker::add(Connection &conn)
{
    Connection *tmp;
    lock();
    _connmap[conn.saddr ^ conn.daddr].push_back(conn);
    tmp = &(_connmap[conn.saddr ^ conn.daddr].back());
    unlock();
    return tmp;
}

void ConnectionTracker::close(Connection &conn)
{
	lock();
	internal_close(conn);
	unlock();
}

void ConnectionTracker::internal_close(Connection &conn)
{
	_connmap[conn.saddr ^ conn.daddr].remove(conn);
}

void ConnectionTracker::internal_close(unsigned long l, Connection &conn)
{
	_connmap[l].remove(conn);
}

Connection * ConnectionTracker::lookup(Connection &conn)
{
	unsigned long l = conn.saddr ^ conn.daddr;
	lock();
	std::list<Connection>::iterator f;
	f = find(_connmap[l].begin(), _connmap[l].end(), conn);
	if(f != _connmap[l].end()) {
		unlock();
		return &(*f);
	}
	unlock();
//	std::cerr << "Not found" << std::endl;
	return NULL;
}

void * ConnectionTracker::main_cycle(void *conn)
{
	cerr << "Thread ceated" << endl;
	ConnectionTracker *tmp = (ConnectionTracker*) conn;
	tmp->warehouse();
	pthread_exit(0);
}

void ConnectionTracker::warehouse()
{
	while(to_exit == 0) {
		sleep(5);
		lock();
		cleanup();
		unlock();
	}
}

void ConnectionTracker::operator() (Connection *conn)
{
	close(*conn);
}

void ConnectionTracker::cleanup()
{
	std::map<unsigned long, std::list<Connection> >::iterator i, iend = _connmap.end();
//	std::list<const Connection &> to_del;

//	to_del.clear();
	for(i = _connmap.begin(); i != iend; ++i) {
		std::list<Connection>::iterator j = i->second.begin(), jend = i->second.end();
		while(j != i->second.end()) {
		    if(j->sent) {
			if(j->exported) {
			    std::cout << "Closing @"<< std::hex <<  (size_t)(&*j)<< std::dec << std::endl;
			    i->second.erase(j++);
			    continue;
			}
		    } else if(j->close_wait == true && (time(NULL) - j->ts > 3)) {
			std::cout << "Sending @"<< std::hex <<  (size_t)(&*j)<< std::dec << std::endl;
			j->exported = false;
			j->sent = true;
			_queue->enqueue(*j);
		    }
		    j++;
		}
/*		for(j=i->second.begin(); j != jend; ++j)
		if(j->close_wait == true && (time(NULL) - j->ts > 20)) {
			_queue->enqueue(*j);
//			to_del.push_back(*j);
			std::cout << "Closing @"<< std::hex <<  (unsigned int)(&*j)<< std::dec << ": " << j->string() << std::endl;
			i->second.erase(j);
//			internal_close(i->first, *j);
			std::cout << "Next!" << std::endl;
		}
		*/
	}

/*	std::list<Connection*>::iterator k, kend = to_del.end();
	for(k = to_del.begin(); k!=kend; ++k) {
		internal_close(**k);	
	}
*/
}

