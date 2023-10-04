#pragma once
#include "HttpError.hpp"
#include "../Colors.hpp"
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <vector>
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
    std::string _body;
    std::vector<char> _bodyVector;
    std::vector<Part> _parts;
    std::string _boundary;
    std::map<std::string, std::string> _errorPages;
    LocationConfig *_locationConfig;
    bool _isFile;

    void fillIncomingRequestMap(std::string const &request);
	int checkContentLength(ServerConfig *config);
    void fillBodyVector(std::vector<char> const &bufferVector);
    void parseLocationConfig();
    void parseIsFile();
    void parsePath(std::string &key, long i);
    void parseIndex();
    void parseMethod();
    void parseRedirection();
    void parseContentType();
    void parseHeaderLine(const std::string& header, std::map<std::string, std::string> &headers);
	void parseHeaderBlockFromString(std::string& inputString, std::string& line, std::map<std::string, std::string> &headers);    
    void parseContentTypeValue();
    void parseVectorParts();
    void parseBody();
    void parseContentDisposition(Part &part);
    bool isContentOfType(std::map<std::string, std::string> headers, std::string typeToMatch);
    bool arePartContentsOfType(std::string typeToMatch);
    void validate();
    void makeOutputFile();
    // utility functions:
    bool isValidMethod(const std::string& method);
    bool isValidResource(const std::string& resource);
    bool isValidHttpVersion(const std::string& httpVersion);
    bool isValidNonNegativeIntegerString(const std::string& str);
    void splitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& output);
    void trimWhitespace(std::string& str);
    std::string stripDoubleQuotes(const std::string& str);
    bool startsWith(const std::string& fullString, const std::string& start);
    void removeFromStringStart(std::string& fullString, const std::string& prefix);
    void printPostUploadRequest();
    void printPartHeaders();
    void printHeaders(std::map<std::string, std::string> map);
    bool hasFileExtension(std::string const &resource);
    bool isDirectory(std::string const &resource);
    bool locationIsSet(std::string const &key);
    void cleanUpMap(std::map<std::string, std::string> _map);

protected:
public:
    HttpRequest();
    HttpRequest(std::string const &request, std::vector<char> &requestVector, ServerConfig *config);
    ~HttpRequest();
    void printRequest();
    LocationConfig *getLocationConfig() const;
    std::string const &getPath();
    void setPath(std::string const &path);
    std::string const &getContentType() const;
    std::string const &getRedirection() const;
    std::string const getHost();
    std::string const getMethod();
    std::string const getResource();
    std::string const getHttpVersion();
    std::string const &getBody() const;
    std::vector<char> const &getBodyVector() const;
	std::string const &getBoundary() const;
    ServerConfig *getConfig() const;
	std::string getContentLength();

};