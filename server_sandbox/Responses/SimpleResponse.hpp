#pragma once
#include "Headers/MovedHeader.hpp"
#include "Headers/OkHeader.hpp"
#include "../Requests/HttpRequest.hpp"
#include <fstream>

class HttpRequest;

class SimpleResponse
{
protected:
    HttpRequest *_request;
    std::string _header;
    std::string _body;
    std::string _response;
    int _bodyLength;
	bool _cgi;

public:
    SimpleResponse();
    SimpleResponse(HttpRequest &request);
    virtual ~SimpleResponse();
    virtual void createResponse(HttpRequest &request) = 0;
    HttpRequest const &getRequest() const;
    std::string const &getHeader() const;
    std::string const &getBody() const;
    std::string const &getResponse();
    int const &getBodyLength() const;
    void setHeader(std::string const &header);
    void setBody(std::string const &body);
    void setBody(std::ifstream &file);
    void updateHeaderOffset(size_t offset);
    void updateBodyOffset(size_t offset);
    int fileLength(std::ifstream &htmlFile);
    void cutRes(std::string& response, size_t i);
	bool isCgi() const;
	void setCgi(bool cgi);

};

