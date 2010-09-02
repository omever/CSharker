#ifndef EXPORTER_H
#define EXPORTER_H

#include <pthread.h>
#include "main.h"
#include "tlvfile.h"
#include "utils.h"
#include "externals/oracle/billing.h"

class Exporter : protected BillingInstance
{
	public:
		Exporter(Billing &, DQueue &);
		~Exporter();
	protected:
		static void * mainloop(void *);
		void warehouse();
	private:
		DQueue * _queue;
		TLVFile _file;
		pthread_t _thread;
		std::string _query;
};

#endif

