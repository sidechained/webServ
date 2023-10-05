#include "OkHeader.hpp"

OkHeader::OkHeader(std::string const &contentType, int const &contentLength) : SimpleHeader()
{
	this->_statusCode = "200";
	this->_status = findStatusCode(this->_statusCode);
	this->_contentType = contentType;
	std::stringstream ss;
	ss << contentLength;
	this->_contentLength = ss.str();
	this->createHeader();
}

OkHeader::~OkHeader()
{
}

void OkHeader::createHeader()
{
	std::stringstream ss;

	ss << this->getStatusLine();
	ss << "Content-Type: " << this->_contentType << this->getCRLF();
	ss << "Content-Length: " << this->_contentLength << this->getCRLF();
	ss << this->getCRLF();
	this->_header = ss.str();
}