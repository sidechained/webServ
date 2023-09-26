#pragma once
#include "HttpError.hpp"
#include "../Colors.hpp"
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include "../Parser/ServerParser.hpp"
#include "../Parser/LocationParser.hpp"
#include "../WebServ.hpp"

// class LocationConfig;

class HttpRequest : public HttpError
{
private:
    std::map<std::string, std::string> _incomingRequest;
    ServerConfig *_config;

    std::string _path;
    std::string _contentType;
    std::string _meth
    std::map<std::string, std::string> _errorPages;
    LocationConfig *_locationConfig;

    void fillIncomingRequestMap(std::string const &request);
    void parseLocationConfig();
    void parsePath(std::string &key, long i);
    void parseIndex();
    void parseMethod();
    void parseRedirection();
    void parseContentType();

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
    LocationConfig *getLocationConfig() const;
    std::string const &getPath();
    void setPath(std::string const &path);
    std::string const &getContentType() const;
    std::string const &getRedirection() const;
    std::string const &getHost() const;
    std::string const &getMethod() const;
    ServerConfig *getConfig() const;
};