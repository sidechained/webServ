#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <cstdlib>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>

# define ERR_OPEN "Unable to open config file"
# define ERR_PARSE "Unable to parse config file"
# define ERR_PARSE_KEY "First line of server block not "
# define ERR_PARSE_EMPTY "Line empty"
# define ERR_PARSE_INDENT "Line not indented by tab"
# define ERR_PARSE_NOKEY "No key found in line"
# define ERR_PARSE_SPACE "Space doesn't follow key"
# define ERR_PARSE_REDIR "Redirections should only contain two elements"
# define ERR_PARSE_SERVER "Server block should end with an empty line"

# include "LocationConfig.hpp"

struct Port
{
	int portNum;
	bool dfault;
};

class ServerConfig
{
	private:
		std::string _line;
		ServerConfig();
		ServerConfig(const ServerConfig &);
		ServerConfig& operator=(const ServerConfig &);
		void extract(std::fstream &configFile);
		void extractKey(std::string &key, std::size_t &colonPos);
		void extractValue(std::string &value, std::size_t &colonPos);
		void extractPorts(std::string portString);
		void extractErrorPages(std::fstream &configFile, bool &gotLine);
		void extractLocations(std::fstream &configFile);
		void detectLine(std::string keyToMatch);
		void errorExit(std::string err1, std::string err2);
		bool countTabIndents(int numTabs);
		void detectAndStripTabIndents(int numTabs);
	public:
		std::string name;
		int client_max_body_size_mb;
		std::vector<Port> ports;
		std::map<std::string, std::string> error_pages;
		std::map<std::string, LocationConfig> locations;
		ServerConfig(std::fstream &configFile);
		~ServerConfig();
		void print();
};

#endif