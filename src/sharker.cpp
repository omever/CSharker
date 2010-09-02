//============================================================================
// Name        : sharker.cpp
// Author      : Grigory Holomiev <omever@gmail.com>
// Version     :
// Copyright   : Published under the terms of the GNU GPL version 2
// Description : Hello World in C, Ansi-style
//============================================================================

#include "Listener.h"
#include "Ethernet.h"
#include "IP.h"
#include "TCP.h"
#include "HTTP.h"

#include <iostream>
using namespace std;

extern unsigned short chksum(void * data, unsigned short len);
extern unsigned short ip_chksum(void *data);
extern unsigned short tcp_chksum(void *data);
extern void terminateRequest(IP &i, TCP &t);

void test()
{
	unsigned char data[66];
	FILE *f = fopen("packet", "r");
	fread(data, 1, 66, f);
	fclose(f);
	Ethernet e;
	IP i;
	TCP t;
	HTTP http;

	data_t d;
	d.value = (const char*)data;
	d.length = 66;

	e.data(d);
	i.data(e.next());
	t.data(i.next());
	terminateRequest(i, t);

	cout << i.i2a(i.dst()) << endl;
};

int main(int argc, char *argv[])
{
	char *dev = NULL;
	if(argc>1)
		dev = argv[1];
	Listener l(dev);
	l.start();
	return 0;
}
