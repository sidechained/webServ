#include "ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser(std::string filename) {
	numServers = 0;
    std::fstream configFile(filename.c_str());
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
	print();
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

void ConfigFileParser::errorExit(std::string err1, std::string err2) {
	std::cerr << err1 << std::endl;
	if (!err2.empty())
		std::cerr << "[" << err2 << "]" << std::endl;
	exit(EXIT_FAILURE);
}

void ConfigFileParser::print() const {
	for (std::vector<ServerConfig>::const_iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it) {
		it->print();
	}
}