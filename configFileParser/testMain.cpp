#include <iostream>
#include <map>
#include "ConfigFileParser.hpp"

// TODO:
// - consider setting defaults after reading not before (i.e. if value empty then fill it)
// - FIX: check paths such as root, uploads, redirection for validity (but are the URI's, paths or what?)

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Wrong number of arguments" << std::endl;
		return EXIT_FAILURE;
	}
	ConfigFileParser cfp(argv[1]);
	// cfp.print(); // convenience method to print everything as a diagnostic:

	std::cout << cfp.numServers << std::endl;
	// NOTE: Presume below that we're using "servers.txt" as the config file...

	// // print whatever we feel like from the vector of serverConfigs:
	// std::cout << std::endl;
	// std::cout << cfp.serverConfigs[0].hostname << std::endl;
	// std::cout << cfp.serverConfigs[1].hostname << std::endl;
	// std::cout << "size:" << cfp.serverConfigs[0].locationConfigs.size() << std::endl;
	// std::cout << cfp.serverConfigs[0].locationConfigs["/eightyTest"].root << std::endl;

	// // lookup a location by path in server 0 (don't forget the preceding slash!)
	// std::cout << std::endl;
	// std::map<std::string, LocationConfig>::iterator itLook = cfp.serverConfigs[0].locationConfigs.find("/eightyTest");
	// if (itLook != cfp.serverConfigs[0].locationConfigs.end()) {
	// 	std::cout << "autoindex: " << (itLook->second.autoindex ? "true" : "false")  << std::endl;
	// } else {
	// 	std::cout << "Key 'eightyTest' not found in the map." << std::endl;
	// }

	// // loop over the servers, looking up and printing some elements
	// std::cout << std::endl;
	// std::vector<ServerParser>::iterator it;
	// std::cout << cfp.serverConfigs.size() << std::endl;
	// for (size_t i = 0; i < cfp.serverConfigs.size(); ++i) {
	// 	std::cout << "Server " << i + 1 << ":\n";
	// 	std::cout << " Name: " << cfp.serverConfigs[i].hostname << ", ";
	// 	if (!cfp.serverConfigs[i].portConfigs.empty())
	// 		std::cout << "First port num: " << cfp.serverConfigs[i].portConfigs[0].number << "\n";
	// 	ServerParser::printLocations(cfp.serverConfigs[i]); // convenience method
	// }
}