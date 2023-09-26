#include "ServerParser.hpp"
#include "GenericParser.hpp"

ServerParser::ServerParser(std::fstream &configFile) {
	setDefaults();
	extract(configFile);
}

ServerParser::ServerParser(const ServerParser &o)
{
	*this = o;
}

ServerParser& ServerParser::operator=(const ServerParser &o)
{
	serverConfig = o.serverConfig;
	return *this;
}

ServerParser::~ServerParser() {

}

void ServerParser::setDefaults()
{
	serverConfig.hostname = DEFAULT_SERVER_HOSTNAME;
	serverConfig.clientMaxBodySizeMB = DEFAULT_SERVER_CLIENTMAXBODYSIZEMB;	
}

void ServerParser::extract(std::fstream &configFile) {
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
			std::cout << line << std::endl;
			errorExit(ERR_PARSE, ERR_PARSE_INDENT_SERVER);
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
			serverConfig.hostname = value;
		} else if (key == "client_max_body_size_mb") {
			extractValue(value, colonPos);
			std::istringstream iss(value);
			iss >> serverConfig.clientMaxBodySizeMB;
		} else if (key == "error_pages") {
			extractErrorPages(configFile);
		} else if (key == "locationConfigs") {
			extractLocations(configFile);
		}
	}
}

void ServerParser::extractPorts(std::string portString) {
	PortConfig portConfig;
	std::istringstream iss(portString);
	int number;
	while (iss >> number) {
		portConfig.number = number;
		portConfig.isDefault = false;
		serverConfig.portConfigs.push_back(portConfig);
	}
}

void ServerParser::extractErrorPages(std::fstream &configFile) {
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
			errorExit(ERR_PARSE, "error_pages entries not indented correctly");
		}
		// here is the expected correct condition (2 tab indents)
		firstLine = false;
		std::string errorKey;
		std::string errorValue;
		std::size_t errorColonPos;
		stripTabIndents(line);
		extractKey(errorKey, errorColonPos);
		extractValue(errorValue, errorColonPos);
		serverConfig.error_pages[errorKey] = errorValue;
	}
}

void ServerParser::extractLocations(std::fstream &configFile) {
	std::getline(configFile, line); // skip 'locationConfigs:' line
	bool firstLine = true;
	while(1)
	{
		if(countTabIndents(line) != 2)
		{
			if (firstLine == true)
			{ // there must be an entry after first line, otherwise syntax error
				errorExit(ERR_PARSE, ERR_NO_VALUE);
			}
			else
			{ // this could be the next entry in the server or the end of the server block (carriage return)
				skipNextLine = true;
				return ;
			}
		}
		// matching condition
		firstLine = false;
		LocationParser locationParser(configFile, line);
		line = locationParser.line; // pick up from where left off
		serverConfig.locationConfigs[locationParser.locationConfig.key] = locationParser.locationConfig;
	}
}

void ServerParser::detectKey(std::string keyToMatch) {
	std::string err_str = std::string(ERR_PARSE_KEY) + "'" + keyToMatch + ":'";
	if(line != keyToMatch + ":")
		errorExit(ERR_PARSE, err_str);
}

void ServerParser::print(ServerConfig serverConfig){
	std::cout << "  Ports:";
	for (std::vector<PortConfig>::const_iterator it = serverConfig.portConfigs.begin(); it != serverConfig.portConfigs.end(); ++it) {
		std::cout << " " << it->number << "(" << (it->isDefault ? "default" : "non-default") << ")";
	}
	std::cout << std::endl;
	std::cout << "  Name: \"" << serverConfig.hostname << "\"" << std::endl;
	std::cout << "  Client Max Body Size (MB): " << serverConfig.clientMaxBodySizeMB << std::endl;
	std::cout << "  Error Pages:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = serverConfig.error_pages.begin(); it != serverConfig.error_pages.end(); ++it) {
		std::cout << "    \"" << it->first << "\": \"" << it->second << "\"" << std::endl;
	}
	printLocations(serverConfig);
}

void ServerParser::printLocations(ServerConfig serverConfig) {
	std::cout << "  Locations:" << std::endl;
	for (std::map<std::string, LocationConfig>::const_iterator it = serverConfig.locationConfigs.begin(); it != serverConfig.locationConfigs.end(); ++it) {
		std::cout << "    \"" << it->first << "\":" << std::endl;
		LocationParser::print(it->second);
	}
}
