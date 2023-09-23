#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
{
}

HttpRequest::~HttpRequest()
{
}

HttpRequest::HttpRequest(std::string const &request, ServerConfig *config) : _config(config)
{
    _noSlash = false;
    fillIncomingRequestMap(request);
    if (getRedirection() != "")
    {
        this->addError("redirection");
        return;
    }
    parseLocationConfig();
    parsePath();
    parseMethod();
    determineContentType();
    cleanUpMap(_incomingRequest);
}

void HttpRequest::fillIncomingRequestMap(std::string const &request)
{
    std::istringstream requestStream(request);
    std::string line;
    std::getline(requestStream, line);
    std::string::size_type pos = line.find(" ");
    if (pos != std::string::npos)
    {
        std::string method = line.substr(0, pos);
        _incomingRequest["Method"] = method;
        std::string::size_type pos2 = line.find(" ", pos + 1);
        if (pos2 != std::string::npos)
        {
            std::string resource = line.substr(pos + 1, pos2 - pos - 1);
            _incomingRequest["Resource"] = resource;
            std::string protocol = line.substr(pos2 + 1);
            _incomingRequest["Protocol"] = protocol;
        }
    }
    while (std::getline(requestStream, line))
    {
        if (line == "\r")
            break;
        std::string::size_type pos = line.find(": ");
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            _incomingRequest[key] = value;
        }
    }
    _incomingRequest["Redirection"] = "";
}

void HttpRequest::cleanUpMap(std::map<std::string, std::string> _map)
{
    for (std::map<std::string, std::string>::iterator it = _map.begin(); it != _map.end(); ++it)
        removeNonPrintableChars(it->second);
}

void HttpRequest::parseLocationConfig()
{
    _path = _incomingRequest["Resource"];
    if (!isDirectory(_path) && !hasFileExtension(_path))
    {
        this->addError("noSlash");
        _noSlash = true;
        return;
    }
    _locationConfig = NULL;
    long i = _path.size();
    while (i > 0)
    {
        PRINT(HTTPREQUEST, BG_RED, "i: " << i)
        // std::cout << BG_RED << "i: " << i << RESET << std::endl;
        std::string key = _path.substr(0, i);
        PRINT(HTTPREQUEST, BG_RED, "key: " << key)
        // std::cout << BG_RED << "key: " << key << RESET << std::endl;
        if (locationIsSet(key))
        {
            _locationConfig = &_config->locations[key];
            PRINT(HTTPREQUEST, BG_RED, "location is set")
            // std::cout << BG_RED << "location is set" << RESET << std::endl;
            if (_locationConfig->root != "")
            {
                if (key == "/")
                    _path = _locationConfig->root + _path;
                else
                    _path = _locationConfig->root + _path.substr(i);
                if (_path[0] == '/')
                    _path = _path.substr(1);
            }
            break;
        }
        PRINT(HTTPREQUEST, BG_RED, "i--" << i)
        // std::cout << BG_RED << "i: " << i << RESET << std::endl;
        i--;
    }
    PRINT(HTTPREQUEST, BG_RED, "got out of the loop" << i << _path)
    // std::cout << BG_RED << "got out of the loop" << i << _path << RESET << std::endl;
}

void HttpRequest::parsePath()
{
    if (isDirectory(_path))
    {
        if (_locationConfig && _locationConfig->index != "")
            _path = _path + _locationConfig->index;
    }
}

void HttpRequest::parseMethod()
{
    std::string requestMethod = _incomingRequest["Method"];

    if (requestMethod == "GET" || requestMethod == "POST" || requestMethod == "DELETE")
    {
        if (!_locationConfig)
            return;
        std::vector<std::string> locationMethods;
        locationMethods = _locationConfig->methods;
        if (std::find(locationMethods.begin(), locationMethods.end(), requestMethod) == locationMethods.end())
            this->addError("methodNotAllowed");
    }
    else
        this->addError("methodNotAllowed");
}

void HttpRequest::determineContentType()
{
    _contentType = findContentType(_path);
}

bool HttpRequest::hasFileExtension(std::string const &resource)
{
    std::string::size_type pos = resource.find(".");
    if (pos != std::string::npos)
        return true;
    return false;
}

bool HttpRequest::isDirectory(std::string const &resource)
{
    if (resource == "/")
        return true;
    if (resource[resource.length() - 1] == '/')
        return true;
    return false;
}

bool HttpRequest::locationIsSet(std::string const &key)
{
    if (_config->locations.find(key) != _config->locations.end())
        return true;
    return false;
}

void HttpRequest::printRequest()
{
    for (std::map<std::string, std::string>::iterator it = _incomingRequest.begin(); it != _incomingRequest.end(); ++it)
    {
        PRINT(HTTPREQUEST, BG_BOLD_MAGENTA, it->first << ": " << RESET << it->second)
        // std::cout << BG_BOLD_MAGENTA << it->first << ": " << RESET << it->second << std::endl;
    }
}

std::string const &HttpRequest::getPath()
{
    return _path;
}

std::string const &HttpRequest::getContentType() const
{
    return _contentType;
}

std::string const &HttpRequest::getRedirection() const
{
    static const std::string emptyString = "";

    if (_locationConfig && !_locationConfig->redirection.empty())
        return _locationConfig->redirection;

    return emptyString;
}

std::string const &HttpRequest::getHost() const
{
    static std::string host = _incomingRequest.at("Host");
    removeNonPrintableChars(host);
    return host;
}

bool HttpRequest::isNoSlash()
{
    return _noSlash;
}