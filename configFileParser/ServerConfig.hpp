#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <cstdlib>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>
# include "LocationConfig.hpp"

struct Port
{
	int portNum;
	bool dfault;
};

class ServerConfig : public GenericConfig
{
	private:
		bool portFlag;
		bool skipNextLine;
		ServerConfig();
		void extract(std::fstream &configFile);
		void extractPorts(std::string portString);
		void extractErrorPages(std::fstream &configFile);
		void extractLocations(std::fstream &configFile);
		void detectKey(std::string keyToMatch);
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
		void printLocations() const;
};

#endif