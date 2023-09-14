#ifndef CONFIGFILEPARSER_HPP
# define CONFIGFILEPARSER_HPP

# include <cstdlib>
# include <vector>
# include <iostream>

# include "ServerConfig.hpp"

class ConfigFileParser : public GenericConfig
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
		void print() const;
};

#endif