#include "ServerConfig.hpp"

ServerConfig::ServerConfig(std::fstream &configFile) {
	name = "";
	client_max_body_size_mb = 1;
	extract(configFile);
}

ServerConfig::~ServerConfig() {

}

void ServerConfig::extract(std::fstream &configFile) {
	std::getline(configFile, _line);
	// _line = server.line?
	std::cout << _line << std::endl;
	detectLine("server");
	while(1) {
		if (_line.empty())
			break;
		std::getline(configFile, _line);
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
	if (!_line.empty())
		errorExit(ERR_PARSE, ERR_PARSE_SERVER);
}

void ServerConfig::extractKey(std::string &key, std::size_t &colonPos) {
	colonPos = _line.find(':');
	if (colonPos == std::string::npos)
		errorExit(ERR_PARSE, ERR_PARSE_NOKEY);
	key = _line.substr(0, colonPos);
}

void ServerConfig::extractValue(std::string &value, std::size_t &colonPos) {
	value = _line.substr(colonPos + 1);
	if (value.empty() || value[0] != ' ')
		errorExit(ERR_PARSE, ERR_PARSE_SPACE);
	value.erase(0, 1); // strip the space
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
	std::getline(configFile, _line);
	while(countTabIndents(2))
	{
		detectAndStripTabIndents(2);
		std::string errorKey;
		std::string errorValue;
		std::size_t errorColonPos;
		extractKey(errorKey, errorColonPos);
		extractValue(errorValue, errorColonPos);
		error_pages[errorKey] = errorValue;
		std::getline(configFile, _line);
	}	
}

void ServerConfig::extractLocations(std::fstream &configFile) {
	std::getline(configFile, _line);
	while(countTabIndents(2))
	{
		LocationConfig location(configFile, _line);
		_line = location.line; // pickup from where location left off
		locations[location.key] = location;
	}
}

void ServerConfig::detectLine(std::string keyToMatch) {
	std::string err_str = std::string(ERR_PARSE_KEY) + "'" + keyToMatch + ":'";
	if(_line != keyToMatch + ":")
		errorExit(ERR_PARSE, err_str);
}

void ServerConfig::errorExit(std::string err1, std::string err2) {
	std::cerr << err1 << std::endl;
	if (!err2.empty())
		std::cerr << "[" << err2 << "]" << std::endl;
	exit(EXIT_FAILURE);
}

bool ServerConfig::countTabIndents(int numTabs) {
	if (_line.empty())
		return false;
	for (int i = 0; i < numTabs; i++)
	{
		if (_line[i] != '\t')
			return false;
	}
	return true;
}

void ServerConfig::detectAndStripTabIndents(int numTabs) {
	std::cout << _line << std::endl;
	if (_line.empty())
		errorExit(ERR_PARSE, ERR_PARSE_EMPTY);
	for (int i = 0; i < numTabs; i++)
	{
		if (_line[i] != '\t')
			errorExit(ERR_PARSE, ERR_PARSE_INDENT);
	}
	_line.erase(0, numTabs);
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
