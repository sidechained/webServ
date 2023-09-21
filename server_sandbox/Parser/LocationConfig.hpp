#pragma once

#include "../WebServ.hpp"
# include <cstdlib>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>


# include "GenericConfig.hpp"
class LocationConfig : public GenericConfig
{
	private:
		bool skipNextLine;
		void extract(std::fstream &configFile);
		void extractMethods(std::string methodString);
	public:
		std::string key;
		std::vector<std::string> methods;
		std::string redirection;
		std::string root;    
		std::string uploads;
		bool autoindex;
		std::string index;
		LocationConfig();
		LocationConfig(std::fstream &configFile, std::string &inLine);
		LocationConfig(const LocationConfig &);
		LocationConfig& operator=(const LocationConfig &);		
		~LocationConfig();
		void print() const;
		void cleanUp();
};
