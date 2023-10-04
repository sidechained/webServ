#include "ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser(std::string filename) {
    std::fstream configFile(filename.c_str());
	if(!configFile.is_open())
		errorExit(ERR_OPEN, "");
	while(1)
	{
		ServerParser serverParser(configFile);
        serverConfigs.push_back(serverParser.serverConfig);
        if (configFile.eof())
			break;
	}
	numServers = serverConfigs.size();
	configFile.close();
	validateIPs();
	validatePorts();
	print();

}

ConfigFileParser::ConfigFileParser(const ConfigFileParser &o)
{
	*this = o;
}

ConfigFileParser& ConfigFileParser::operator=(const ConfigFileParser &o)
{
	serverConfigs = o.serverConfigs;
	numServers = o.numServers;
	return *this;
}

ConfigFileParser::~ConfigFileParser()
{
	
}

// check for duplicate IPs across server configs
void ConfigFileParser::validateIPs() {
    std::vector<std::string> seenIPs;  // Vector to store seen port numbers
	for (unsigned int i = 0; i < serverConfigs.size(); i++)
	{
		std::string ip = serverConfigs[i].ip;
		bool isDuplicate = false;
		for (std::vector<std::string>::iterator seenIt = seenIPs.begin(); seenIt != seenIPs.end(); ++seenIt) {
			if (*seenIt == ip) {
				isDuplicate = true;
				break;
			}
		}
		if (isDuplicate) {
			std::cout << "Duplicate port number '" << ip << "' found in server config, exiting" << std::endl;
			exit(EXIT_FAILURE);
		} 
		seenIPs.push_back(ip);
	}	
}

// check for and remove duplicate ports in a server config
void ConfigFileParser::validatePorts() {
    std::vector<int> seenPortNumbers;  // Vector to store seen port numbers
	for (unsigned int i = 0; i < serverConfigs.size(); i++)
	{
		for (std::vector<PortConfig>::iterator it = serverConfigs[i].portConfigs.begin(); it != serverConfigs[i].portConfigs.end(); /* increment inside loop */) {
			int portNumber = it->number;
			// Check if the port number has been seen before
			bool isDuplicate = false;
			for (std::vector<int>::iterator seenIt = seenPortNumbers.begin(); seenIt != seenPortNumbers.end(); ++seenIt) {
				if (*seenIt == portNumber) {
					isDuplicate = true;
					break;
				}
			}

			if (isDuplicate) {
				// Remove duplicate port number by erasing the element and advancing the iterator
				std::cout << "Duplicate port number " << portNumber << " found in server config, removing..." << std::endl;
				it = serverConfigs[i].portConfigs.erase(it);
			} else {
				// Add the port number to the vector of seen port numbers
				seenPortNumbers.push_back(portNumber);
				// Move to the next port config
				++it;
			}
		}
	}	
}

void ConfigFileParser::print() const {
	int serverNum = 0;
	for (std::vector<ServerConfig>::const_iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it) {
		std::cout << "Server " << serverNum << ":" << std::endl;
		ServerParser::print(*it);
		serverNum++;
	}
}