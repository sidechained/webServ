#include "HttpRequest.hpp"

HttpRequest::HttpRequest(std::string const &request, ServerConfig *config) : _incomingRequest(request), _config(config)
{
    _noSlash = false;
    extractRequestFields();
    determineResource();
    determineContentType();
    cleanUp();
}

void HttpRequest::cleanUp()
{
    for (std::map<std::string, std::string>::iterator it = _request.begin(); it != _request.end(); ++it)
        removeNonPrintableChars(it->second);
}

HttpRequest::HttpRequest()
{
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
    _request["Redirection"] = "";
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

void HttpRequest::determineResource()
{
    std::string resource = _request["Resource"];

    // Return if the resource is not a directory and does not have a file extension
    if (!isDirectory(resource) && !hasFileExtension(resource))
    {
        _noSlash = true;
        return;
    }

    std::string key, index, root, redirection;
    bool autoindex;
    std::vector<std::string> methods;
    
    long i = resource.size();

    std::cout << BG_RED << "Looping trough resource from the end to the beginning to find the correct root, index and redirection" << RESET << std::endl;
    while (i > 0)
    {
        key = resource.substr(0, i);
        std::cout << BG_RED << "key: " << key << RESET << std::endl;
        if (locationIsSet(key))
        {
            redirection = _config->locations[key].redirection;
            root = _config->locations[key].root;
            index = _config->locations[key].index;
            autoindex = _config->locations[key].autoindex;
            methods = _config->locations[key].methods;
            break;
        }
        i--;
    }

    if (redirection != "")
    {
        _request["Redirection"] = redirection;
        return;
    }

    if (root != "")
    {
        if (key == "/")
            resource = root + resource;
        else
            resource = root + resource.substr(i);

        if (resource[0] == '/')
            resource = resource.substr(1);

        std::cout << RED << "Found root for: " << resource << RESET << std::endl;
    }

    if (isDirectory(resource))
    {
        std::cout << BG_GREEN << "resource is a directory: " << resource << RESET << std::endl;
        std::cout << BG_GREEN << "index: " << index << RESET << std::endl;

        if (index != "")
            resource = resource + index;
    }

    _request["Resource"] = resource;
}

// void HttpRequest::determineResource()
// {
//     std::string resource = _request["Resource"];
//     if (resource == "/")
//     {
//         std::map<std::string, LocationConfig>::iterator itLook = _config->locations.find("/");
//         if (itLook != _config->locations.end())
//         {
//             resource = _config->locations["/"].index;
//             _request["Resource"] = "/" + resource;
//         }
//         else
//         {
//             std::cout << "Key '/' not found in the map." << std::endl;
//         }
//     }

//     std::cout << BG_YELLOW << "resource: " << resource << _config->locations[resource].root << RESET << std::endl;

//     if (_config->locations.find(resource) != _config->locations.end())
//     {
//         if (!_config->locations[resource].root.empty())
//             _request["Resource"] = _config->locations[resource].root;
//     }
// }

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

std::string const &HttpRequest::getRedirection() const
{
    return _request.at("Redirection");
}

bool HttpRequest::isNoSlash() const
{
    return _noSlash;
}