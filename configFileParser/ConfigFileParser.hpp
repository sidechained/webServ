#ifndef CONFIGFILEPARSER_HPP
# define CONFIGFILEPARSER_HPP

# include <cstdlib>
# include <vector>
# include <iostream>

# include "ServerConfig.hpp"
class ConfigFileParser
{
	private:
		ConfigFileParser();
	public:
		ConfigFileParser(std::string filename);
		ConfigFileParser(const ConfigFileParser &);
		ConfigFileParser& operator=(const ConfigFileParser &);
		~ConfigFileParser();
		std::vector<ServerConfig> serverConfigs;
		int numServers;
		void errorExit(std::string err1, std::string err2);
		void print() const;
};

#endif