#include "ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser(std::string filename)
{
    std::fstream configFile(filename.c_str());
	if(!configFile.is_open())
		errorExit(ERR_OPEN, ""); // "" correct way to handle?
	while(1)
	{
		ServerConfig serverConfig(configFile);
        serverConfigs.push_back(serverConfig);		
        if (configFile.eof())
			break;
	}
	// printServers(servers);
	configFile.close();
}

ConfigFileParser::~ConfigFileParser()
{

}

void ConfigFileParser::ErrorExit(std::string err1, std::string err2)
{
	std::cerr << err1 << std::endl;
	if (!err2.empty())
		std::cerr << "[" << err2 << "]" << std::endl;
	exit(EXIT_FAILURE);
}