#include "ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser(std::string filehostname) {
	numServers = 0;
    std::fstream configFile(filehostname.c_str());
	if(!configFile.is_open())
		errorExit(ERR_OPEN, ""); // "" correct way to handle?
	while(1)
	{
		ServerConfig serverConfig(configFile);
        serverConfigs.push_back(serverConfig);
		numServers++;
        if (configFile.eof())
			break;
	}
	configFile.close();
}

ConfigFileParser::ConfigFileParser(const ConfigFileParser &o)
{
	*this = o;
}

ConfigFileParser& ConfigFileParser::operator=(const ConfigFileParser &o)
{
	serverConfigs = o.serverConfigs;
	numServers = o.numServers;
	return *this;
}


ConfigFileParser::~ConfigFileParser()
{
	
}

void ConfigFileParser::print() const {
	int serverNum = 0;
	for (std::vector<ServerConfig>::const_iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it) {
		std::cout << "Server " << serverNum << ":" << std::endl;
		it->print();
		serverNum++;
	}
}