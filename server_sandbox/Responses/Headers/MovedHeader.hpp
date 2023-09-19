#pragma once
#include "SimpleHeader.hpp"

class MovedHeader : public SimpleHeader
{
private:
	/* data */
public:
	MovedHeader(std::string const &host, std::string const &resource);
	~MovedHeader();
	void createHeader();
};
