#pragma once
#include "HttpError.hpp"
#include "../Colors.hpp"
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include "../Parser/ServerConfig.hpp"
#include "../Parser/LocationConfig.hpp"
#include "../WebServ.hpp"

class LocationConfig;

class HttpRequest : public HttpError
{
private:
    std::map<std::string, std::string> _incomingRequest;
    ServerConfig *_config;

    bool _noSlash;
    std::string _path;
    std::string _contentType;
    LocationConfig *_locationConfig;

    void fillIncomingRequestMap(std::string const &request);
    void parseLocationConfig();
    void parsePath();
    void parseMethod();
    void determineContentType();
    bool hasFileExtension(std::string const &resource);
    bool isDirectory(std::string const &resource);
    bool locationIsSet(std::string const &key);
    void cleanUpMap(std::map<std::string, std::string> _map);

protected:
public:
    HttpRequest();
    HttpRequest(std::string const &request, ServerConfig *config);
    ~HttpRequest();
    void printRequest();
    std::string const &getPath();
    std::string const &getContentType() const;
    std::string const &getRedirection() const;
    std::string const &getHost() const;
    bool isNoSlash();
};