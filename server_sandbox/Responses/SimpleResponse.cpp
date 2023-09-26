#include "SimpleResponse.hpp"

SimpleResponse::SimpleResponse() : _request(), _header(""), _body(""), _response(""), _bodyLength(0)
{
}

SimpleResponse::SimpleResponse(HttpRequest &request) : _request(&request), _header(""), _body(""), _response(""), _bodyLength(0)
{
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
   _bodyLength = _body.size();
}

void SimpleResponse::setBody(std::ifstream &file)
{
    std::ostringstream oss2;
    oss2 << file.rdbuf();
    std::string body = oss2.str();
    setBody(body);
}


void SimpleResponse::updateHeaderOffset(size_t offset)
{
    _header = _header.substr(offset);
}

void SimpleResponse::updateBodyOffset(size_t offset)
{
    _body = _body.substr(offset);

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

void SimpleResponse::cutRes(std::string& response, size_t i)
{
    _response = response.substr(i);
}