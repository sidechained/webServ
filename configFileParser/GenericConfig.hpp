#ifndef GENERICCONFIG_HPP
# define GENERICCONFIG_HPP

# include <cstdlib>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>

# define ERR_PARSE "Unable to parse config file"
// # define ERR_PARSE_EMPTY "Line empty"
# define ERR_PARSE_INDENT "Line not indented by tab"
# define ERR_PARSE_NOKEY "No key found in line"
# define ERR_PARSE_SPACE "Space doesn't follow key"

class GenericConfig
{
	protected:
		void extractKey(std::string &key, std::size_t &colonPos);
		void extractValue(std::string &value, std::size_t &colonPos);
		void errorExit(std::string err1, std::string err2);
		bool countTabIndents(int numTabs);
		void detectAndStripTabIndents(int numTabs);
	public:
		std::string line; // public because serverConfig needs to revisit the line that the location ended at
};

#endif