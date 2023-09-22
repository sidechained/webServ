#include "CGIUploader.hpp"
#include "../configFileParser/ConfigFileParser.hpp"

// "serverConfig.txt", "postRequest.txt", "tmp/output.txt"
int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "Three arguments required: server config file, input file (post request) and output file" << std::endl;
		exit(EXIT_FAILURE);
	}
    std::string serverConfigFilename(argv[1]);
	std::string postRequestFilename(argv[2]);
	std::string dataOutputFilename(argv[3]);
    ConfigFileParser configFileParser(serverConfigFilename);
    ServerConfig serverConfig = configFileParser.serverConfigs[0];
	std::cout << std::endl;
	CGIUploader uploader(serverConfig, postRequestFilename, dataOutputFilename);
}