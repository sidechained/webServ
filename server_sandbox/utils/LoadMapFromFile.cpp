#include "../WebServ.hpp"

void loadMapFromFile(const std::string &filename, std::map<std::string, std::string> &map)
{
    std::ifstream file(filename.c_str());
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value))
            {
                map[key] = value;
            }
        }
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file " << filename << std::endl;
    }
}
