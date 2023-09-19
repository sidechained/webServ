#include "SimpleHeader.hpp"

SimpleHeader::SimpleHeader() : _header(""), _status(""), _contentType(""), _contentLength(""), _location(""), _httpVersion("HTTP/1.1")
{
}

SimpleHeader::~SimpleHeader()
{
}

std::string const &SimpleHeader::getHeader() const
{
	return this->_header;
}
