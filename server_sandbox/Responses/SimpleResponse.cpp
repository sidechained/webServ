#include "SimpleResponse.hpp"

SimpleResponse::SimpleResponse() : _request(), _header(""), _body(""), _response(""), _bodyLength(0), _headerSent(false), _bodySent(false)
{
}

SimpleResponse::SimpleResponse(HttpRequest &request) : _request(&request), _header(""), _body(""), _response(""), _bodyLength(0), _headerSent(false), _bodySent(false)
{
    createResponse();
}

SimpleResponse::~SimpleResponse()
{
}

HttpRequest const &SimpleResponse::getRequest() const
{
    return *_request;
}

std::string const &SimpleResponse::getHeader() const
{
    return _header;
}

std::string const &SimpleResponse::getBody() const
{
    return _body;
}

int const &SimpleResponse::getBodyLength() const
{
    return _bodyLength;
}

void SimpleResponse::setHeader(std::string const &header)
{
    _header = header;
}

void SimpleResponse::setBody(std::string const &body)
{
    _body = body;
}

void SimpleResponse::setHeaderSent(bool headerSent)
{
    _headerSent = headerSent;
}

void SimpleResponse::setBodyLength(int length)
{
    _bodyLength = length;
}

bool SimpleResponse::isHeaderSent() const
{
    return _headerSent;
}

void SimpleResponse::setBodySent(bool bodySent)
{
    _bodySent = bodySent;
}

bool SimpleResponse::isBodySent() const
{
    return _bodySent;
}

void SimpleResponse::updateHeaderOffset(size_t offset)
{
    _header = _header.substr(offset);
    if (_header.empty())
        _headerSent = true;
}

void SimpleResponse::updateBodyOffset(size_t offset)
{
    _body = _body.substr(offset);
    if (_body.empty())
        _bodySent = true;
}

void SimpleResponse::createResponse()
{
}

std::string const &SimpleResponse::getResponse() 
{
    _response = _header + _body;
    return _response;
}

int SimpleResponse::fileLength(std::ifstream &file)
{
    file.seekg(0, std::ios::end);
    int fileLength = file.tellg();
    file.seekg(0, std::ios::beg);

    return fileLength;
}

void SimpleResponse::printResponse()
{
    std::cout << BG_BOLD_MAGENTA << "Header: " << RESET << getHeader() << std::endl;
    std::cout << BG_BOLD_MAGENTA << "Body: " << RESET << getBody() << std::endl;
}

void SimpleResponse::cutRes(std::string& response, size_t i)
{
    _response = response.substr(i);
    //std::cout << BG_BOLD_MAGENTA << "RESPONSE UPDATED size: " << _response.size() << RESET << std::endl;
}