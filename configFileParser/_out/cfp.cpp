void ConfigFileParser::printServers() {
	for (size_t i = 0; i < servers.size(); ++i) {
		printServer(i);
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