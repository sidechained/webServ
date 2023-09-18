#include "ServerManager.hpp"

ServerManager::ServerManager(){}
ServerManager::~ServerManager(){}

void    ServerManager::setupServers()
{
    std::cout << std::endl;
    std::cout << BG_RED << "Initializing servers..." << RESET << std::endl;

    for (std::vector<SimpleServer>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
        if (!serverDub)
            it->setupServer();
        Logger::logMsg(LIGHTMAGENTA, CONSOLE_OUTPUT, "Server Created: ServerName[%s] Host[%s] Port[%d]",it->getServerName().c_str(),
                inet_ntop(AF_INET, &it->getHost(), buf, INET_ADDRSTRLEN), it->getPort());
    }
}