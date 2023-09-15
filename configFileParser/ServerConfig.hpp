#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <cstdlib>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>
# include "LocationConfig.hpp"

# define ERR_PARSE_KEY "First line of server block not "
// # define ERR_PARSE_SERVER "Server block should end with an empty line"

struct Port
{
	int portNum;
	bool dfault;
};

class ServerConfig : public GenericConfig
{
	private:
		bool skipGetLine;
		ServerConfig();
		void extract(std::fstream &configFile);
		void extractPorts(std::string portString);
		void extractErrorPages(std::fstream &configFile);
		void extractLocations(std::fstream &configFile);
		void detectLine(std::string keyToMatch);
	public:
		std::string name;
		int client_max_body_size_mb;
		std::vector<Port> ports;
		std::map<std::string, std::string> error_pages;
		std::map<std::string, LocationConfig> locations;
		ServerConfig(std::fstream &configFile);
		// ServerConfig(const ServerConfig &);
		// ServerConfig& operator=(const ServerConfig &);
		~ServerConfig();
		void print() const;
};

#endif