#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <cstdlib>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>
# include "LocationParser.hpp"

struct PortConfig
{
	int number;
	bool isDefault;
};

struct ServerConfig
{
	std::string hostname;
	unsigned int clientMaxBodySizeMB;
	std::vector<PortConfig> portConfigs;
	std::map<std::string, std::string> error_pages;
	std::map<std::string, LocationConfig> locationConfigs;
};

class ServerParser : public GenericParser
{
	private:
		bool portFlag;
		bool skipNextLine;
		ServerParser();
		void setDefaults();
		void extract(std::fstream &configFile);
		void extractPorts(std::string portString);
		void extractErrorPages(std::fstream &configFile);
		void extractLocations(std::fstream &configFile);
		void detectKey(std::string keyToMatch);
	public:
		ServerConfig serverConfig;
		ServerParser(std::fstream &configFile);
		ServerParser(const ServerParser &);
		ServerParser& operator=(const ServerParser &);
		~ServerParser();
		static void print(ServerConfig serverConfig);
		static void printLocations(ServerConfig serverConfig);
};

#endif