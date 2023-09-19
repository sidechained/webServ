#include "../WebServ.hpp"
#include "../Colors.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

void loadContentTypesFromFile(const std::string &filehostname, std::map<std::string, std::string> &_contentTypes)
{
    std::ifstream file(filehostname.c_str());
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string extension, contentType;
            if (std::getline(iss, extension, '=') && std::getline(iss, contentType))
            {
                _contentTypes[extension] = contentType;
            }
        }
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file " << filehostname << std::endl;
    }
}

std::string findContentType(const std::string &resource)
{
    static std::map<std::string, std::string> _contentTypes;
    if (_contentTypes.empty())
    {
        loadContentTypesFromFile("./config/content_types.txt", _contentTypes);
    }
    if (resource.empty() || resource == "/")
        return "text/html";
    size_t lastDotPosition = resource.find_last_of(".");
    if (lastDotPosition != std::string::npos)
    {
        std::string extension = resource.substr(lastDotPosition);
        std::map<std::string, std::string>::iterator it = _contentTypes.find(extension);
        if (it != _contentTypes.end())
        {
            return it->second;
        }
    }
    std::cout << BG_BOLD_RED << "No extension found" << RESET << std::endl;
    return "text/html";
}
