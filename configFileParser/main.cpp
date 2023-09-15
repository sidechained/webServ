#include "ConfigFileParser.hpp"

#define ERR_OPEN "Unable to open config file"

void errorExit(std::string err1, std::string err2) {
	std::cerr << err1 << std::endl;
	if (!err2.empty())
		std::cerr << "[" << err2 << "]" << std::endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Wrong number of arguments" << std::endl;
		return EXIT_FAILURE;
	}
    // std::fstream configFile(argv[1]);
	// if(!configFile.is_open())
	// 	errorExit(ERR_OPEN, ""); // "" correct way to handle?
	// LocationConfig lc(configFile);
	// lc.print();

	// ServerConfig sc(configFile);
	// sc.print();

	ConfigFileParser cfp(argv[1]);
	// cfp.print();
	// create and loop over array of servers, initialising them with the 
	// std::vector<PollingServer> pollingServers;

	// for(size_t i = 0; i < config.getNumServers(); i++)
	// {
	// 	// for now just printing the server info here!
	// 	config.printServer(i);
		
	// 	// PollingServer(config.servers[i]) pollingServer;
	// 	// pollingServers.push_back(pollingServer);
	// }

	// configFile.close();
}