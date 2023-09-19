#include "OkHeader.hpp"

OkHeader::OkHeader(std::string const &contentType, int const &contentLength) : SimpleHeader()
{
	this->_status = "200 OK";
	this->_contentType = contentType;
	this->_contentLength = std::to_string(contentLength);
	this->createHeader();
}

OkHeader::~OkHeader()
{
}

void OkHeader::createHeader()
{
	std::stringstream ss;

	ss << this->_httpVersion << " " << this->_status << "\r\n";
	ss << "Content-Type: " << this->_contentType << "\r\n";
	ss << "Content-Length: " << this->_contentLength << "\r\n";
	ss << "\r\n";
	this->_header = ss.str();
}