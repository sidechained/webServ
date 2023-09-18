#include "ServerManager.hpp"

ServerManager::ServerManager(){}
ServerManager::~ServerManager(){}

void    ServerManager::setupServers()
{
    std::cout << std::endl;
    std::cout << BG_RED << "Initializing servers..." << RESET << std::endl;

	std::vector<int> ports;
    ports.push_back(8080);
	SimpleServer* server1 = new SimpleServer(AF_INET, SOCK_STREAM, 0, ports, "127.0.0.2", MAX_CLIENTS);
    _servers.push_back(server1);
    //_servers.push_back(SimpleServer(AF_INET, SOCK_STREAM, 0, ports, "127.0.0.3", MAX_CLIENTS));

}

/*void    ServerManager::initializeSets()
{
    FD_ZERO(&_recv_fd_pool);
    FD_ZERO(&_write_fd_pool);

    // adds servers sockets to _recv_fd_pool set
    for(std::vector<SimpleServer>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
		std::vector<ListeningSocket> sockets = it->getSockets();
		for(std::vector<ListeningSocket>::iterator it2 = sockets.begin(); it2 != sockets.end(); ++it2)
		{
			int sock = it2->getSock();
			if (listen(sock, MAX_CLIENTS) == -1)
			{
				//Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: listen error: %s   Closing....", strerror(errno));
				exit(EXIT_FAILURE);
			}
			if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
			{
				//Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: fcntl error: %s   Closing....", strerror(errno));
				exit(EXIT_FAILURE);
			}
			addToSet(sock, _recv_fd_pool);
			_servers_map.insert(std::make_pair(sock, *it));
			
			if (sock > _biggest_fd)
				_biggest_fd = sock;
		}
    }
	std::cout << "server initialized" << std::endl;
}*/

void	ServerManager::addToSet(const int i, fd_set &set)
{
    FD_SET(i, &set);
    if (i > _biggest_fd)
        _biggest_fd = i;
}

void    ServerManager::runServers()
{
    /*fd_set  recv_set_cpy;
    fd_set  write_set_cpy;
    int     select_ret;*/

    _biggest_fd = 0;
	std::cout << "run" << std::endl;
    //initializeSets();
}