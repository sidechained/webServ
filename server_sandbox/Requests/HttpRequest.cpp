#include "HttpRequest.hpp"

HttpRequest::HttpRequest(std::string const &request) : _incomingRequest(request)
{
    extractRequestFields();
    determineResource();
    determineContentType();
}

HttpRequest::~HttpRequest()
{
}

void HttpRequest::extractRequestFields()
{
    std::istringstream requestStream(_incomingRequest);
    std::string line;
    std::getline(requestStream, line);
    std::string::size_type pos = line.find(" ");
    if (pos != std::string::npos)
    {
        std::string method = line.substr(0, pos);
        _request["Method"] = method;
        std::string::size_type pos2 = line.find(" ", pos + 1);
        if (pos2 != std::string::npos)
        {
            std::string resource = line.substr(pos + 1, pos2 - pos - 1);
            _request["Resource"] = resource;
            std::string protocol = line.substr(pos2 + 1);
            _request["Protocol"] = protocol;
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
            _request[key] = value;
        }
    }

}

void HttpRequest::determineResource()
{
    std::string resource = _request["Resource"];
    if (resource == "/")
        resource = "/index.html";
    _request["Resource"] = resource;
}

void HttpRequest::determineContentType()
{
    std::string resource = _request["Resource"];
    _request["Content-Type"] = findContentType(resource);
}

void HttpRequest::printRequest()
{
    for (std::map<std::string, std::string>::iterator it = _request.begin(); it != _request.end(); ++it)
    {
        std::cout << BG_BOLD_MAGENTA << it->first << ": " << RESET << it->second << std::endl;
    }
}

std::map<std::string, std::string> &HttpRequest::getRequest()
{
    return _request;
}

std::string const &HttpRequest::getResource()
{
    return _request["Resource"];
}

std::string const &HttpRequest::getMethod()
{
    return _request["Method"];
}

std::string const &HttpRequest::getContentType() const
{
    return _request.at("Content-Type");

}