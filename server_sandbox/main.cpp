#include "WebServ.hpp"
#include <csignal>
#include <iostream>
#include <netinet/in.h>  // for AF_INET
#include <sys/socket.h>  // for SOCK_STREAM
#include "Servers/PollingServer.hpp"
#include "Servers/ServerManager.hpp"
#include "Parser/ConfigFileParser.hpp"


bool isRunning = true;

void sigIntHandler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << "SIGINT received" << std::endl;
		isRunning = false;
	}
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;
    signal(SIGINT, sigIntHandler);
	signal(SIGPIPE, SIG_IGN);
    /*std::vector<int> ports;
    ports.push_back(8080);
    PollingServer server(AF_INET, SOCK_STREAM, 0, ports, "0.0.0.0", 10);
    server.launch();*/
	//std::vector<int> ports;
    //ports.push_back(8080);
	ConfigFileParser cfp(argv[1]);

	ServerManager manager;

	manager.setupServers();
	manager.runServers();
    std::cout << BG_BLUE << "Server finished" << RESET << std::endl;
    return 0;
}
