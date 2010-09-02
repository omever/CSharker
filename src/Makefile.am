bin_PROGRAMS = csharker
csharker_CXXFLAGS = -DDEBUG -g -O0 $(ORACLE_OCI_CFLAGS)
csharker_CFLAGS = -DDEBUG -g -O0
csharker_LDFLAGS = -DDEBUG -g -O0 -locci $(ORACLE_OCI_LDFLAGS)
# find *  -path testing -prune -or -name \*.cpp -printf "%p " -or -name \*.h -printf "%p " && echo
csharker_SOURCES = sharker.cpp \
				   Ethernet.cpp Ethernet.h \
				   Listener.cpp Listener.h \
				   IP.h IP.cpp \
				   TCP.h TCP.cpp \
				   Packet.h Packet.cpp \
				   HTTP.h HTTP.cpp \
				   conntrack.h conntrack.cpp \
				   FTP.h FTP.cpp \
				   connection.h connection.cpp \
				   exporter.h exporter.cpp \
				   tsqueue.h main.h \
				   OSCAR.h OSCAR.cpp \
				   tlvfile.h tlvfile.cpp \
				   Protocol.h \
				   externals/oracle/billing.h externals/oracle/billing.cpp \
				   utils.h utils.cpp
csharker_LDADD = -lpcap -lpthread
