#ifndef GENERICCONFIG_HPP
# define GENERICCONFIG_HPP

# include <cstdlib>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>

// all error messages are here at the moment for convenience

// GenericParser:
# define ERR_PARSE "Unable to parse config file"
# define ERR_PARSE_INDENT "Line not indented by tab"
# define ERR_PARSE_NOKEY "No key found in line"
# define ERR_PARSE_SPACE "Space doesn't follow key"
# define ERR_PARSE_SYNTAX "Syntax error"

// LocationParser:
# define ERR_PARSE_REDIR "Redirections should only contain two elements"

// ServerParser:
# define ERR_PARSE_KEY "First line of server block not "
# define ERR_PARSE_PORTS "Server cannot be created without specifying at least one port to listen on"

// ConfigFileParser
# define ERR_OPEN "Unable to open config file"

class GenericParser
{
	protected:
		void extractKey(std::string &key, std::size_t &colonPos);
		void extractValue(std::string &value, std::size_t &colonPos);
		void errorExit(std::string err1, std::string err2);
		int countTabIndents(const std::string line);
		void stripTabIndents(std::string &line);
	public:
		std::string line; // public because serverConfig needs to revisit the line that the location ended at
};

#endif