#include <fstream>
#include <iostream>
#include "tlvfile.h"
#include "connection.h"

using namespace std;

TLVFile::TLVFile(std::string filename)
	: fout(filename.c_str(), std::ios_base::app)
{
	fout << "Hi there" << std::endl;
}

TLVFile::~TLVFile()
{
	fout.close();
}

bool TLVFile::append(const Connection &data)
{
	fout << data;
	fout << endl;
}
