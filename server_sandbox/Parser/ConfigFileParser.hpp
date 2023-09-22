#pragma once

# include <cstdlib>
# include <vector>
# include <iostream>
# include "ServerConfig.hpp"
# include "GenericConfig.hpp"

class ServerConfig;

class ConfigFileParser : public GenericConfig
{
	private:
		ConfigFileParser();
	public:
		ConfigFileParser(std::string filehostname);
		ConfigFileParser(const ConfigFileParser &);
		ConfigFileParser& operator=(const ConfigFileParser &);
		~ConfigFileParser();
		std::vector<ServerConfig> serverConfigs;
		int numServers;
		void print() const;
};