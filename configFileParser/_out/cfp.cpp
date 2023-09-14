void ConfigFileParser::printServers() {
	for (size_t i = 0; i < servers.size(); ++i) {
		printServer(i);
	}
}

void ConfigFileParser::extractKey(std::string line, std::string &key, std::size_t &colonPos) {
	colonPos = line.find(':');
	if (colonPos == std::string::npos)
		error_exit(ERR_PARSE, ERR_PARSE_NOKEY);
	key = line.substr(0, colonPos);
}

void ConfigFileParser::extractValue(std::string line, std::string &value, std::size_t &colonPos) {
	value = line.substr(colonPos + 1);
	if (value.empty() || value[0] != ' ')
		error_exit(ERR_PARSE, ERR_PARSE_SPACE);
	value.erase(0, 1); // strip the space
}

void ConfigFileParser::extractPorts(std::string portString, std::vector<Port> &ports)
{
	Port port;
	std::istringstream iss(portString);
	int portNum;
	while (iss >> portNum) {
		port.portNum = portNum;
		port.dfault = false;
		ports.push_back(port);
	}
}

size_t ConfigFileParser::getNumServers()
{
	return servers.size();
}

const std::vector<ServerConfig> ConfigFileParser::getServers()
{
	return servers;
}