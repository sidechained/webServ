#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <cstdlib>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>

# define ERR_PARSE_REDIR "Redirections should only contain two elements"

struct Redirection
{
	std::string oldURL;
	std::string newURL;
};

# include "GenericConfig.hpp"
class LocationConfig : public GenericConfig
{
	private:
		void extract(std::fstream &configFile);
		void extractMethods(std::string methodString);
		void extractRedir(std::string redirString);
	public:
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