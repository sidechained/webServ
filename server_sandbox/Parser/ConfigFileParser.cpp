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
	validatePorts();
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

void ConfigFileParser::validatePorts() {
    std::vector<int> seenPortNumbers;  // Vector to store seen port numbers
    for (std::vector<PortConfig>::iterator it = serverConfigs[0].portConfigs.begin(); it != serverConfigs[0].portConfigs.end(); /* increment inside loop */) {
        int portNumber = it->number;

		std::cout << "here2" << std::endl;
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
            it = serverConfigs[0].portConfigs.erase(it);
        } else {
            // Add the port number to the vector of seen port numbers
            seenPortNumbers.push_back(portNumber);
            // Move to the next port config
            ++it;
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