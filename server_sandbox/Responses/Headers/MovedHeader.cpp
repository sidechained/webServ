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
	this->_status = "301 Moved Permanently";
	this->_location = "http://" + host + resource + "/";
	this->_contentLength = "0";
	this->_location = removeNonPrintableChars(this->_location);
	this->createHeader();
}

void MovedHeader::createHeader()
{
	std::ostringstream oss;

	oss << this->_httpVersion << " " << this->_status << "\r\n";
	oss << "Location: " << this->_location << "\r\n";
	oss << "Content-Length: " << this->_contentLength << "\r\n";
	oss << "\r\n";

	this->_header = oss.str();
}