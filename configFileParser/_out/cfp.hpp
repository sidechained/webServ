#ifndef CONFIGFILEPARSER_HPP
# define CONFIGFILEPARSER_HPP

# include <cstdlib>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>

# include "ServerConfig.hpp"

# define ERR_OPEN "Unable to open config file"
# define ERR_PARSE "Unable to parse config file"
# define ERR_PARSE_KEY "First line of server block not "
# define ERR_PARSE_EMPTY "Line empty"
# define ERR_PARSE_INDENT "Line not indented by tab"
# define ERR_PARSE_NOKEY "No key found in line"
# define ERR_PARSE_SPACE "Space doesn't follow key"
# define ERR_PARSE_REDIR "Redirections should only contain two elements"
# define ERR_PARSE_SERVER "Server block should end with an empty line"

class ServerConfig;

class ConfigFileParser
{
	private:
		std::string line;
		std::vector<ServerConfig> servers;
		std::fstream configFile;

		ConfigFileParser(ConfigFileParser &);
		ConfigFileParser& operator=(ConfigFileParser &);
		void printRedirection(const Redirection& redirection);
		void printLocation(const Location& location);

		void error_exit(std::string err1, std::string err2);
		void detect_line(std::string &line, std::string keyToMatch);
		bool count_tab_indents(int numTabs);
		void DetectAndStripTabIndents(int numTabs);
		void init_location(Location &location);
		void extractKey(std::string line, std::string &key, std::size_t &colonPos);
		void extractValue(std::string line, std::string &value, std::size_t &colonPos);
		void extract_ports(std::string portString, std::vector<Port> &ports);
		void extractMethods(std::string methodString, Location &location);
		void extractRedir(std::string redirString, Location &location);
		void extract_locations(ServerConfig &server, std::fstream &configFile, std::string &line, bool &gotLine);
		void extract_error_pages(ServerConfig &server, std::fstream &configFile, std::string &line, bool &gotLine);
		void extract_server();
	public:
		ConfigFileParser(const std::string filename);
		~ConfigFileParser();
		size_t getNumServers();
		const std::vector<ServerConfig> getServers();
		void printServer(int i);
		void printServers();
};

#endif