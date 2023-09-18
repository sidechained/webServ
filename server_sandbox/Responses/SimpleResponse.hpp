#pragma once
#include "../Requests/HttpRequest.hpp"
#include <fstream>


class SimpleResponse
{
private:
    HttpRequest _request;
    std::string _header;
    std::string _body;
    int _bodyLength;
    bool _headerSent;
    bool _bodySent;
public:
    SimpleResponse();
    SimpleResponse(HttpRequest &request);
    virtual ~SimpleResponse();
    virtual void createResponse();
    HttpRequest const &getRequest() const;
    std::string const &getHeader() const;
    std::string const &getBody() const;
    std::string const &getResponse() const;
    int const &getBodyLength() const;
    void setHeader(std::string const &header);
    void setBody(std::string const &body);
    void setHeaderSent(bool headerSent);
    void setBodyLength(int length);
    bool isHeaderSent() const;
    void setBodySent(bool bodySent);
    bool isBodySent() const;
    void updateHeaderOffset(size_t offset);
    void updateBodyOffset(size_t offset);
    int fileLength(std::ifstream &htmlFile);
};
