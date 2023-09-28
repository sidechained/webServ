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

struct ContentDisposition {
	std::string type;
	std::string name;
	std::string filename;
};

struct Part {
	std::map<std::string, std::string> headers;
	ContentDisposition contentDisposition;
	std::string data;
};

class HttpRequest : public HttpError
{
private:
    std::map<std::string, std::string> _incomingRequest;
    ServerConfig *_config;

    std::string _path;
    std::string _contentType;
    // std::string _meth;
    std::string _body;
    std::vector<Part> parts;
    std::string boundary;
    std::map<std::string, std::string> _errorPages;
    LocationConfig *_locationConfig;

    void fillIncomingRequestMap(std::string const &request);
    void parseLocationConfig();
    void parsePath(std::string &key, long i);
    void parseIndex();
    void parseMethod();
    void parseRedirection();
    void parseContentType();
    void parseContentTypeValue();
    void parseVectorParts();

    void splitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& output);
    void trimWhitespace(std::string& str);
    std::string stripDoubleQuotes(const std::string& str);
    bool startsWith(const std::string& fullString, const std::string& start);
    void removeFromStringStart(std::string& fullString, const std::string& prefix);

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
    std::string const getHost();
    std::string const getMethod();
    std::string const &getBody() const;
    ServerConfig *getConfig() const;
};