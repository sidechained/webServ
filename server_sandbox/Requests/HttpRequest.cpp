#include "HttpRequest.hpp"

HttpRequest::HttpRequest(std::string const &request, ServerConfig *config) : _incomingRequest(request), _config(config)
{
    _noSlash = false;
    extractRequestFields();
    determineResource();
    determineContentType();
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
    std::cout << BG_YELLOW << "resource: " << resource << RESET << std::endl;
    // if resource ends without a slash and it is not a file, set _noSlash to true
    if (!isDirectory(resource) && !hasFileExtension(resource))
    {
        _noSlash = true;
        return;
    }

    // if (!hasFileExtension(resource) && !isDirectory(resource))
    //     resource = resource + "/";
    // std::cout << BG_YELLOW << "resource after adding slash: " << resource << RESET << std::endl;
    if (isDirectory(resource))
    {
        std::cout << BG_YELLOW << "resource is a directory" << RESET << std::endl;
        std::string key = resource;
        if (key.length() > 1 && key[key.length() - 1] == '/')
            key = key.substr(0, key.length() - 1);
        if (locationIsSet(key))
        {
            std::cout << BG_YELLOW << "resource is a directory and is in the map: " << _config->locations[resource].index << RESET << std::endl;
            resource = resource + _config->locations[key].index;
        }
    }
    std::cout << BG_YELLOW << "new resource: " << resource << RESET << std::endl;

    for(long i = resource.size(); i >= 0; i--)
    {
        std::string key = resource.substr(0, i);
        if (key.length() > 1 && key[key.length() - 1] == '/')
            key = key.substr(0, key.length() - 1);
        std::cout << BG_YELLOW << key << RESET << std::endl;
        if (locationIsSet(key))
        {
            std::cout << BG_YELLOW << "resource is a directory and is in the map: " << _config->locations[resource].index << RESET << std::endl;
             if (!_config->locations[key].root.empty())
             {
                std::cout << YELLOW << "root: " << _config->locations[key].root << RESET << std::endl;
                std::cout << GREEN << "resource: " << resource << " i: " << i << RESET << std::endl;
                if (key == "/")
                    resource = _config->locations[key].root + resource;
                else
                    resource = _config->locations[key].root + "/" + resource.substr(i);
                std::cout << BLUE << "new resource: " << resource << RESET << std::endl;
                if (resource[0] == '/')
                    resource = resource.substr(1);
                std::cout << RED << "new resource: " << resource << RESET << std::endl;
                break;
             }
        }
    }

    //check if location has root
    //search for location in map starting with the string before the last slash and ending with the last slash
    //if found, append the index to the resource
    //if not found, append the index to the resource
    


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

bool HttpRequest::isNoSlash() const
{
    return _noSlash;
}