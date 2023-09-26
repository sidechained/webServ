#include "SimpleHeader.hpp"

SimpleHeader::SimpleHeader() : _header(""), _status(""), _statusCode(""),_contentType(""), _contentLength(""), _location(""), _httpVersion("HTTP/1.1")
{
}

SimpleHeader::~SimpleHeader()
{
}

std::string const &SimpleHeader::getHeader() const
{
	return this->_header;
}

std::string const SimpleHeader::getStatusLine() const
{
	std::string statusLine = this->_httpVersion + " " + this->_statusCode + " " + this->_status + "\r\n";
	return statusLine;
}

std::string const SimpleHeader::getCRLF() const
{
	std::string crlf = "\r\n";
	return crlf;
}