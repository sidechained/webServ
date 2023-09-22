#include "ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser(std::string filename) {
    std::fstream configFile(filename.c_str());
	if(!configFile.is_open())
		errorExit(ERR_OPEN, "");
	while(1)
	{
		ServerParser serverParser(configFile);
        serverConfigs.push_back(serverParser.serverConfig);
        if (configFile.eof())
			break;
	}
	numServers = serverConfigs.size();
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
		ServerParser::print(*it);
		serverNum++;
	}
}