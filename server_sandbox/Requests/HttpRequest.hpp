#pragma once
#include "../Sockets/ListeningSocket.hpp"
#include "../Colors.hpp"
#include <string>
#include <sstream>
#include <map>
#include "../WebServ.hpp"
#include "../Parser/ServerConfig.hpp"

class HttpRequest
{
private:
    std::string _incomingRequest;
    std::map<std::string, std::string> _request;
    bool _noSlash;
    ServerConfig *_config;
    void extractRequestFields();
    void determineResource();
    void determineContentType();
    bool hasFileExtension(std::string const &resource);
    bool isDirectory(std::string const &resource);
    bool locationIsSet(std::string const &key);

protected:
public:
    HttpRequest();
    HttpRequest(std::string const &request, ServerConfig *config);
    ~HttpRequest();
    void printRequest();
    std::map<std::string, std::string> &getRequest();
    std::string const &getResource();
    std::string const &getMethod();
    std::string const &getContentType() const;
    bool isNoSlash() const;
    void cleanUp();
};