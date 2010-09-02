#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <string>

#define IFEXIST(x) (x.size()?x.at(0):"")

typedef unsigned long long size64;

class convertErrorException : public std::exception
{
    virtual const char * what() const throw()
    {
	return "Data conversion error";
    }
};

template <class T>
bool from_string(T& t, 
                 const std::string& s)
{
  std::istringstream iss(s);
  return !(iss >> t).fail();
}

template <class T>
T from_string(const std::string& s)
{
  T t;
  std::istringstream iss(s);
  if((iss >> t).fail()) {
//    throw convertException_ex;
    throw;
  }
 
  return t;
}

template <class T>
std::string & to_string(std::string& s, T t)
{
  std::ostringstream iss(s);
  if((iss << t).fail()) {
//    throw convertException_ex;
    throw;
  }
 
  return s;
}

template <class T>
T string_cast(const std::string& s)
{
	T t;
	std::istringstream iss(s);
	iss >> t;
	return t;
}

std::string ip2num(const std::string &src);

#endif
