#pragma once
#include "SimpleHeader.hpp"

class OkHeader : public SimpleHeader
{
private:
	/* data */
public:
	OkHeader(std::string const &contentType, int const &contentLength);
	~OkHeader();
	void createHeader();
};
