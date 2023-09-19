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
    // Get the requested resource from the HTTP request
    std::string resource = _request["Resource"];
    std::cout << BG_YELLOW << "resource: " << resource << RESET << std::endl;

    // Check if the resource doesn't end with a slash and is not a file
    if (!isDirectory(resource) && !hasFileExtension(resource))
    {
        // Set _noSlash to true and return
        _noSlash = true;
        return;
    }

    std::string index;
    std::string root;
    std::string redirection;

    //loop trough resource from the end to the beginning to find the correct root, index and redirection
    // std::cout << BG_RED << "Looping trough resource from the end to the beginning to find the correct root, index and redirection" << RESET << std::endl;
    // for (long i = resource.size(); i >= 0; i--)
    // {
    //     std::string key = resource.substr(0, i);
    //     std::cout << BG_RED << "key: " << key << RESET << std::endl;
    //     if (locationIsSet(key))
    //     {
    //         index = _config->locations[key].index;
    //         root = _config->locations[key].root;
    //         redirection = _config->locations[key].redirection;
    //     }
    // } 

    // If the resource is a directory
    if (isDirectory(resource))
    {
        std::cout << BG_YELLOW << "resource is a directory" << RESET << std::endl;
        std::string key = resource;

        // Remove trailing slash if it exists
        if (key.length() > 1 && key[key.length() - 1] == '/')
            key = key.substr(0, key.length() - 1);

        // Check if there is a location configuration for this directory
        if (locationIsSet(key))
        {
            // Update the resource by appending the index from the location configuration
            std::cout << BG_BLUE  << "Appending index to direcotry resource" << std::endl;
            resource = resource + _config->locations[key].index;
            std::cout << BG_BLUE << "resource: " << resource << std::endl;
        }
        else
        {
            // If there is no location configuration for this directory, append "index.html" to the resource
            std::cout << BG_BLUE << "Appending index.html to direcotry resource" << std::endl;
            resource = resource + "index.html";
            std::cout << BG_BLUE << "resource: " << resource << std::endl;
        }
    }

    // Loop to find the correct root for the resource
    for (long i = resource.size(); i >= 0; i--)
    {
        std::string key = resource.substr(0, i);

        // Remove trailing slash if it exists
        if (key.length() > 1 && key[key.length() - 1] == '/')
            key = key.substr(0, key.length() - 1);

        // Check if there is a location configuration for this directory
        if (locationIsSet(key))
        {
            std::cout << BG_YELLOW << "Found location configuration for: " << key << RESET << std::endl;
            std::cout << BG_YELLOW << _config->locations[key].root << RESET << std::endl;


            if (!_config->locations[key].redirection.empty())
            {
                std::cout << BG_YELLOW << "Found redirection for: " << key << RESET << std::endl;
                std::cout << BG_YELLOW << _config->locations[key].redirection << RESET << std::endl;
                _request["Redirection"] = _config->locations[key].redirection;
                return;
            }
            // If a root is specified in the location configuration
            if (!_config->locations[key].root.empty())
            {
                std::cout << YELLOW << "root: " << _config->locations[key].root << RESET << std::endl;
                std::cout << GREEN << "resource: " << resource << " i: " << i << RESET << std::endl;

                // Update the resource path by combining the root and the remaining path
                if (key == "/")
                    resource = _config->locations[key].root + resource;
                else
                    resource = _config->locations[key].root + "/" + resource.substr(i);

                std::cout << BLUE << "new resource: " << resource << RESET << std::endl;

                // Remove a leading slash if it exists
                if (resource[0] == '/')
                    resource = resource.substr(1);

                std::cout << RED << "new resource: " << resource << RESET << std::endl;

                // Break out of the loop
                break;
            }
        }
    }

    // Update the "Resource" field in the HTTP request with the final resource path
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