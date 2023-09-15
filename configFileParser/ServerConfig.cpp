#include "ServerConfig.hpp"
#include "GenericConfig.hpp"

ServerConfig::ServerConfig(std::fstream &configFile) {
	name = "";
	client_max_body_size_mb = 1;
	skipGetLine = false;
	extract(configFile);
}

ServerConfig::~ServerConfig() {

}

void ServerConfig::extract(std::fstream &configFile) {
	std::getline(configFile, line);
	detectLine("server");
	while(1) {
		if(skipGetLine == false)
			std::getline(configFile, line);
		else
			skipGetLine = false;
		if (line.empty()) // an empty line signifies the end of the server block (or EOF?)
		{	
			std::cout << "movingon..." << std::endl;
			break;
		}
		detectAndStripTabIndents(1);
		std::string key;
		std::string value;
		std::size_t colonPos;
		extractKey(key, colonPos);
		if (key == "ports") {
			extractValue(value, colonPos);
			extractPorts(value);
		} else if (key == "name") {
			extractValue(value, colonPos);
			name = value;
		} else if (key == "client_max_body_size_mb") {
			extractValue(value, colonPos);
			std::istringstream iss(value);
			iss >> client_max_body_size_mb;
		} else if (key == "error_pages") {
			extractErrorPages(configFile);
		} else if (key == "locations") {
			extractLocations(configFile);
		}
	}
	// if (!line.empty())
	// 	errorExit(ERR_PARSE, ERR_PARSE_SERVER);
}

void ServerConfig::extractPorts(std::string portString) {
	Port port;
	std::istringstream iss(portString);
	int portNum;
	while (iss >> portNum) {
		port.portNum = portNum;
		port.dfault = false;
		ports.push_back(port);
	}
}

void ServerConfig::extractErrorPages(std::fstream &configFile) {
	while(1)
	{
		std::getline(configFile, line);
		if(!countTabIndents(2))
			break;
		detectAndStripTabIndents(2); // we don't need to detect here now, just strip
		std::string errorKey;
		std::string errorValue;
		std::size_t errorColonPos;
		extractKey(errorKey, errorColonPos);
		extractValue(errorValue, errorColonPos);
		error_pages[errorKey] = errorValue;
	}
	skipGetLine = true;
}

void ServerConfig::extractLocations(std::fstream &configFile) {
	std::cout << line << std::endl;
	std::getline(configFile, line); // skip 'locations:' line
	while(1)
	{
		std::cout << line << std::endl;
		if(!countTabIndents(2))
			break;
		LocationConfig location(configFile, line);
		line = location.line; // serverConfig picks from where locationConfig left off
		locations[location.key] = location;
	}
}

void ServerConfig::detectLine(std::string keyToMatch) {
	std::string err_str = std::string(ERR_PARSE_KEY) + "'" + keyToMatch + ":'";
	if(line != keyToMatch + ":")
		errorExit(ERR_PARSE, err_str);
}

void ServerConfig::print() const {
	std::cout << "  Ports:";
	for (std::vector<Port>::const_iterator it = ports.begin(); it != ports.end(); ++it) {
		std::cout << " " << it->portNum << "(" << (it->dfault ? "default" : "non-default") << ")";
	}
	std::cout << std::endl;
	std::cout << "  Name: \"" << name << "\"" << std::endl;
	std::cout << "  Client Max Body Size (MB): " << client_max_body_size_mb << std::endl;
	std::cout << "  Error Pages:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
		std::cout << "    \"" << it->first << "\": \"" << it->second << "\"" << std::endl;
	}
	std::cout << "  Locations:" << std::endl;
	for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		std::cout << "    \"" << it->first << "\":" << std::endl;
		it->second.print();
	}
}
