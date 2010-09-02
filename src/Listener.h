#ifndef LISTENER_H
#define LISTENER_H

#include <pcap.h>
#include "externals/oracle/billing.h"

class Listener
{
	public:
		Listener(const char *dev);
		~Listener();
		void start();
	private:
		pcap_t *h;
		Billing _bill;
};
#endif

