#include "../WebServ.hpp"
#include "../Colors.hpp"


std::string findContentType(const std::string &resource)
{
    static std::map<std::string, std::string> _contentTypes;
    if (_contentTypes.empty())
        loadMapFromFile("./config/content_types.txt", _contentTypes);
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
