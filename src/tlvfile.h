#ifndef TLVFILE_H
#define TLVFILE_H

#include <fstream>
#include "connection.h"

class TLVFile
{
	public:
		TLVFile(std::string filename);
		~TLVFile();
		bool append(const Connection &data);
	private:
		std::ofstream fout;
	protected:
};

#endif

