#pragma once
#include "SimpleHeader.hpp"
#include "../../WebServ.hpp"

class MovedHeader : public SimpleHeader
{
private:
public:
	MovedHeader(std::string const &host, std::string const &resource);
	~MovedHeader();
	void createHeader();
};
