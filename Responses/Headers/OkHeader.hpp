#pragma once
#include "SimpleHeader.hpp"
#include "../../WebServ.hpp"


class OkHeader : public SimpleHeader
{
private:
public:
	OkHeader(std::string const &contentType, int const &contentLength);
	~OkHeader();
	void createHeader();
};
