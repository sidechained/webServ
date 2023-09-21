#include "ServerConfig.hpp"
#include "GenericConfig.hpp"

ServerConfig::ServerConfig(std::fstream &configFile) {
	hostname = "";
	client_max_body_size_mb = 1;
	extract(configFile);
	print();
	cleanUp();
}

void ServerConfig::cleanUp() {
	removeNonPrintableChars(hostname);
	for (std::map<std::string, std::string>::iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
		removeNonPrintableChars(it->second);
	}
	for (std::map<std::string, LocationConfig>::iterator it = locations.begin(); it != locations.end(); ++it) {
		it->second.cleanUp();
	}
}

ServerConfig::~ServerConfig() {

}



void ServerConfig::extract(std::fstream &configFile) {
	skipNextLine = false;
	portFlag = false;
	std::getline(configFile, line);
	detectKey("server");
	while(1) {
		if (skipNextLine == false)
			std::getline(configFile, line);
		else
			skipNextLine = false;
		if (line.empty() || configFile.eof())
		{	
			if (portFlag == true)
				return ;
			else
				errorExit(ERR_PARSE, ERR_PARSE_PORTS);
		}
		if(countTabIndents(line) != 1)
		{
			errorExit(ERR_PARSE, ERR_PARSE_SYNTAX);
		}
		stripTabIndents(line);
		std::string key;
		std::string value;
		std::size_t colonPos;
		extractKey(key, colonPos);
		if (key == "ports") {
			extractValue(value, colonPos);
			extractPorts(value);
			portFlag = true;
		} else if (key == "hostname") {
			extractValue(value, colonPos);
			hostname = value;
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
}

void ServerConfig::extractPorts(std::string portString) {
	Port port;
	std::istringstream iss(portString);
	int number;
	while (iss >> number) {
		port.number = number;
		port.dfault = false;
		ports.push_back(port);
	}
}

void ServerConfig::extractErrorPages(std::fstream &configFile) {
	bool firstLine = true;
	while(1)
	{
		// here we can have one of three things:
		// - a 2-tab indented key/value entry (error page)
		// - a 1-tab indented key/value entry (next entry in server block)
		// - a carriage return or EOF indicating the end of the server block
		std::getline(configFile, line);
		if (firstLine == false && (countTabIndents(line) == 1 || line.empty() || configFile.eof()))
		{ 
			// if the indent count is 1 or line is empty we will return and skip reading of next line
			// empty detection will be done by 
			// this doesn't apply to first line as that MUST be an entry otherwise error
			skipNextLine = true;
			return;
		}
		if (countTabIndents(line) != 2)
		{
			errorExit(ERR_PARSE, ERR_PARSE_SYNTAX);
		}
		// here is the expected correct condition (2 tab indents)
		firstLine = false;
		std::string errorKey;
		std::string errorValue;
		std::size_t errorColonPos;
		stripTabIndents(line);
		extractKey(errorKey, errorColonPos);
		extractValue(errorValue, errorColonPos);
		error_pages[errorKey] = errorValue;
	}
}

void ServerConfig::extractLocations(std::fstream &configFile) {
	std::getline(configFile, line); // skip 'locations:' line
	bool firstLine = true;
	while(1)
	{
		if(countTabIndents(line) != 2)
		{
			if (firstLine == true)
			{ // there must be an entry after first line, otherwise syntax error
				errorExit(ERR_PARSE, ERR_PARSE_SYNTAX);
			}
			else
			{ // this could be the next entry in the server or the end of the server block (carriage return)
				skipNextLine = true;
				return ;
			}
		}
		// matching condition
		firstLine = false;
		LocationConfig location(configFile, line);
		line = location.line; // pick up from where locationConfig left off
		locations[location.key] = location;
	}
}

void ServerConfig::detectKey(std::string keyToMatch) {
	std::string err_str = std::string(ERR_PARSE_KEY) + "'" + keyToMatch + ":'";
	if(line != keyToMatch + ":")
		errorExit(ERR_PARSE, err_str);
}

void ServerConfig::print() const {
	std::cout << "  Ports:";
	for (std::vector<Port>::const_iterator it = ports.begin(); it != ports.end(); ++it) {
		std::cout << " " << it->number << "(" << (it->dfault ? "default" : "non-default") << ")";
	}
	std::cout << std::endl;
	std::cout << "  hostname: \"" << hostname << "\"" << std::endl;
	std::cout << "  Client Max Body Size (MB): " << client_max_body_size_mb << std::endl;
	std::cout << "  Error Pages:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
		std::cout << "    \"" << it->first << "\": \"" << it->second << "\"" << std::endl;
	}
	printLocations();
}

void ServerConfig::printLocations() const {
	std::cout << "  Locations:" << std::endl;
	for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		std::cout << "    \"" << it->first << "\":" << std::endl;
		it->second.print();
	}
}
