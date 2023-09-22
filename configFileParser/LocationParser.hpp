#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <cstdlib>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>

# include "GenericParser.hpp"

struct LocationConfig
{
	std::string key;
	std::vector<std::string> methods;
	std::string redirection;
	std::string root;    
	std::string uploads;
	bool autoindex;
	std::string index;
};

class LocationParser : public GenericParser
{
	private:
		bool skipNextLine;
		void setDefaults();
		void extract(std::fstream &configFile);
		void extractMethods(std::string methodString);
	public:
		LocationConfig locationConfig;
		LocationParser();
		LocationParser(std::fstream &configFile, std::string &inLine);
		LocationParser(const LocationParser &);
		LocationParser& operator=(const LocationParser &);		
		~LocationParser();
		static void print(LocationConfig locationConfig);
};

#endif