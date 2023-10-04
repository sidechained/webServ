#include "WebServ.hpp"
#include <csignal>
#include <iostream>
#include <netinet/in.h> // for AF_INET
#include <sys/socket.h> // for SOCK_STREAM
#include "Parser/ConfigFileParser.hpp"
#include "Colors.hpp"
#include "Servers/ServerManager.hpp"
#define DEFAULT_CONFIG_FILE_PATH "configs/servers.txt"

bool isRunning = true;

void sigIntHandler(int sig)
{
    if (sig == SIGINT)
    {
        std::cout << "SIGINT received" << std::endl;
        isRunning = false;
    }
}

// int main(int argc, char const *argv[])
// {
//     if (argc != 2)
// 	{
// 		std::cout << "Wrong number of arguments" << std::endl;
// 		return EXIT_FAILURE;
// 	}
// 	ConfigFileParser cfp(argv[1]);

//     signal(SIGINT, sigIntHandler);
//     signal(SIGPIPE, SIG_IGN);
//     std::vector<int> ports;
//     ports.push_back(8080);
//     PollingServer server(cfp.serverConfigs[0]);
//     //PollingServer server(AF_INET, SOCK_STREAM, 0, ports, "0.0.0.0", 10);
//     server.launch();
//     std::cout << BG_BLUE << "Server finished" << RESET << std::endl;
//     return 0;
// }

int main(int argc, const char *argv[])
{
    // todolist changes start here >>>
    if (argc < 1 || argc > 2)
    {
        std::cout << "Wrong number of arguments" << std::endl;
        return EXIT_FAILURE;
    }
    const char *path;
    if (argc == 1)
    {
    	path = DEFAULT_CONFIG_FILE_PATH;
    }
    if (argc == 2)
    {
    	path = argv[1];
    }
    ConfigFileParser cfp(path);
    // todolist changes end here <<<    	
    signal(SIGINT, sigIntHandler);
    signal(SIGPIPE, SIG_IGN);
    /*std::vector<int> ports;
    ports.push_back(8080);
    PollingServer server(AF_INET, SOCK_STREAM, 0, ports, "0.0.0.0", 10);
    server.launch();*/
    // std::vector<int> ports;
    // ports.push_back(8080);

    ServerManager manager = ServerManager(&cfp);

    manager.setupServers();
    manager.runServers();
    std::cout << BG_BLUE << "Server finished" << RESET << std::endl;
    return 0;
}
