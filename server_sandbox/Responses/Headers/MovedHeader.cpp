#include "MovedHeader.hpp"

std::string removeNonPrintableChars(const std::string& input) {
    std::string result;
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        if (c >= 32 && c <= 126) {
            result += c;
        }
    }
    return result;
}

MovedHeader::~MovedHeader()
{
}

MovedHeader::MovedHeader(std::string const &host, std::string const &resource)
{
	this->_statusCode = "301";
	this->_status = findStatusCode(this->_statusCode);
	this->_location = "http://" + host + resource + "/";
	this->_contentLength = "0";
	this->_location = removeNonPrintableChars(this->_location);
	this->createHeader();
}

void MovedHeader::createHeader()
{
	std::ostringstream oss;

	oss << this->getStatusLine();
	oss << "Location: " << this->_location << this->getCRLF();
	oss << "Content-Length: " << this->_contentLength << this->getCRLF();
	oss << this->getCRLF();

	this->_header = oss.str();
}