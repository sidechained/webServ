#include "../WebServ.hpp"
#include <string>
#include <map>

std::string findStatusCode(std::string const &code)
{
    static std::map<std::string, std::string> statusCodes;
    if (statusCodes.empty())
        loadMapFromFile("./config/status_codes.txt", statusCodes);
    if (code.empty())
        return "200 OK";
    std::map<std::string, std::string>::iterator it = statusCodes.find(code);
    if (it != statusCodes.end())
    {
        return it->second;
    }
    return "200 OK";
}