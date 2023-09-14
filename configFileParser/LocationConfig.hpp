#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

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

struct Redirection
{
	std::string oldURL;
	std::string newURL;
};

class LocationConfig
{
	private:
		void extract(std::fstream &configFile);
		void extractKey(std::string &key, std::size_t &colonPos);
		void extractValue(std::string &value, std::size_t &colonPos);
		void extractMethods(std::string methodString);
		void extractRedir(std::string redirString);
		void errorExit(std::string err1, std::string err2);
		bool countTabIndents(int numTabs);
		void detectAndStripTabIndents(int numTabs);	
	public:
		std::string line; // public because serverConfig needs to revisit the line that the location ended at
		std::string key;
		std::vector<std::string> methods;
		Redirection redirection;
		std::string alias;    
		std::string uploads;
		bool autoindex;
		std::string index;
		LocationConfig();
		LocationConfig(std::fstream &configFile, std::string &inLine);
		LocationConfig(const LocationConfig &);
		LocationConfig& operator=(const LocationConfig &);		
		~LocationConfig();
		void print() const;
};

#endif