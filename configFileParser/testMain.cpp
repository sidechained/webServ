#include <iostream>
#include <map>
#include "ConfigFileParser.hpp"

// TODO:
// - consider setting defaults after reading not before (i.e. if value empty then fill it)
// - what element from subject needed to be added to config file?
// - revise error messages
// - FIX: can have keys but no values (e.g. serversBlankPorts.txt should generate an error)
// - FIX: value can be more than one element i.e. redir: /old-url /new-url

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Wrong number of arguments" << std::endl;
		return EXIT_FAILURE;
	}
	ConfigFileParser cfp(argv[1]);
	
	// convenience method to print everything as a diagnostic:
	// cfp.print();

	// NOTE: Presume below that we're using "servers.txt" as the config file...


	// print whatever we feel like from the vector of serverConfigs:
	std::cout << std::endl;
	std::cout << cfp.serverConfigs[0].name << std::endl;
	std::cout << cfp.serverConfigs[1].name << std::endl;
	std::cout << "size:" << cfp.serverConfigs[0].locations.size() << std::endl;
	std::cout << cfp.serverConfigs[0].locations["/eightyTest"].alias << std::endl;

	// lookup a location by path in server 0 (don't forget the preceding slash!)
	std::cout << std::endl;
	std::map<std::string, LocationConfig>::iterator itLook = cfp.serverConfigs[0].locations.find("/eightyTest");
	if (itLook != cfp.serverConfigs[0].locations.end()) {
		std::cout << "autoindex: " << (itLook->second.autoindex ? "true" : "false")  << std::endl;
	} else {
		std::cout << "Key 'eightyTest' not found in the map." << std::endl;
	}

	// loop over the servers, looking up and printing some elements
	std::cout << std::endl;
	std::vector<ServerConfig>::iterator it;
	for (size_t i = 0; i < cfp.serverConfigs.size(); ++i) {
		std::cout << "Server " << i + 1 << ":\n";
		std::cout << " Name: " << cfp.serverConfigs[i].name << ", ";
		if (!cfp.serverConfigs[i].ports.empty())
			std::cout << "First port num: " << cfp.serverConfigs[i].ports[0].portNum << "\n";
		cfp.serverConfigs[i].printLocations(); // convenience method
	}
}