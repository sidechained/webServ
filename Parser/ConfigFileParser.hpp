#ifndef CONFIGFILEPARSER_HPP
# define CONFIGFILEPARSER_HPP

# include <cstdlib>
# include <vector>
# include <iostream>
# include "ServerParser.hpp"

class ConfigFileParser : public GenericParser
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
		void validateIPs();
		void validatePorts();
		void print() const;
};

#endif