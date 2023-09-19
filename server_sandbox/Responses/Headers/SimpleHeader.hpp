#pragma once
#include <string>
#include <sstream>

class SimpleHeader
{
protected:
	std::string _header;
	std::string _status;
	std::string _contentType;
	std::string _contentLength;
	std::string _location;
	std::string _httpVersion;
public:
	SimpleHeader();
	virtual ~SimpleHeader() = 0;
	virtual void createHeader() = 0;
	std::string const &getHeader() const;
};
