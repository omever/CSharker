#ifndef OSCAR_H
#define OSCAR_H

#include <string>
#include <list>
#include "Packet.h"

class OSCAR: public Packet
{
	public:
		OSCAR(data_t);
		OSCAR();
		virtual void parse();
		void clear();

		virtual Packet* nextProto();
		virtual std::string pack();
	protected:
		static unsigned char _u8(const char *data);
		static unsigned short _u16(const char *data);
		static unsigned short _u16l(const char *data);
		static unsigned int _u32(const char *data);
		static unsigned int _u32l(const char *data);
		unsigned char u8();
		unsigned short u16();
		unsigned short u16l();
		unsigned int u32();
		unsigned int u32l();
		std::string string(size_t len);
		std::string string08();
		std::string string16();

		void parseSNAC();
		bool parseFLAP();
		bool parse1();
		bool parse2();
		bool parse3();
		bool parse4();
		bool parse5();
		void parseF4();
		void parseF4S6();
		void parseF4S6C1(std::string &);
		void parseF4S6C2(std::string &);
		void parseF4S7();
		void dump(size_t);
		void dump(std::string &);
		void tlvBlock();
		bool utf8_valid(std::string &);
		static std::string conv_1251_utf8(std::string &);
		static std::string conv_ucs2_utf8(std::string &);
		virtual void output(std::ostream &os);
	private:
		std::list<std::string> _data;
		unsigned short length;
		unsigned short family;
		unsigned short subtype;
		unsigned short flags;
		unsigned int reqid;
};

#endif

