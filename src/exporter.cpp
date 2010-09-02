#include "exporter.h"
#include <iostream>
#include <string>
#include <occi.h>
using namespace std;
using namespace oracle::occi;

Exporter::Exporter(Billing &bill, DQueue &queue)
	: BillingInstance(bill), _file(std::string("output.data"))
{
	_queue = &queue;
	_query = "INSERT INTO csharker_log (ipaddress1, ipaddress2, ts, tsize, tstart, tstop, data) VALUES (:1, :2, SYSDATE, :4, :5, :6, :7)";
	pthread_create(&_thread, NULL, Exporter::mainloop, this);
};

Exporter::~Exporter()
{
	pthread_join(_thread, NULL);
}


void* Exporter::mainloop(void *data)
{
	Exporter *e = static_cast<Exporter*>(data);
	e->warehouse();
}

void Exporter::warehouse()
{
	::Connection *s = NULL;
	Statement *sth = NULL;
	while(1) {
		if((s=_queue->dequeue()) != NULL) {
			if(_conn == NULL) {
				continue;
			}

			try {
			    if(sth == NULL) {
				sth = _conn->createStatement(_query);
				sth->setAutoCommit(true);
			    }
			    //std::cout << *s << std::endl;
			    sth->setInt(1, s->saddr);
			    sth->setInt(2, s->daddr);
			    sth->setInt(3, s->bytes);
			    sth->setInt(4, s->start);
			    sth->setInt(5, s->ts);
			    sth->setString(6, s->conn_data != NULL? s->conn_data->pack() : "");
			    sth->executeUpdate();
			}
			catch(std::exception &e) {
			    std::cerr << "Error executing query: " << e.what() << std::endl;
			    delete(sth);
			    sth = NULL;
			}
			s->setExported(true);
		} else {
			sleep(1);
		}
	}
}
