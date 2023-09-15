#pragma once
#include "../Sockets/ListeningSocket.hpp"
#include "../Colors.hpp"
#include <string>
#include <sstream>
#include <map>
#include "../WebServ.hpp"

class HttpRequest
{
private:
    std::string _incomingRequest;
    std::map<std::string, std::string> _request;
    void extractRequestFields();
    void determineResource();
    void determineContentType();

protected:
public:
    HttpRequest(std::string const &request);
    ~HttpRequest();
    void printRequest();
    std::map<std::string, std::string> &getRequest();
    std::string const &getResource();
    std::string const &getMethod();
    std::string const &getContentType() const;
};