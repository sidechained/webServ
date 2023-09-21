#include "Server.hpp"

Server::Server(int domain, int type, int protocol, std::vector<int> ports, std::string ip, int backlog)
{
	for (unsigned long i = 0; i < ports.size(); i++)
	{
		_sockets.push_back(new Socket(domain, type, protocol, ports[i], ip, backlog));
	}
}

Server::Server(ServerConfig &config)
{
    for (unsigned long i = 0; i < config.ports.size(); i++)
    {
        _sockets.push_back(new Socket(AF_INET, SOCK_STREAM, 0, config.ports[i].number, config.hostname, 10));
    }
    _config = &config;
}


Server::~Server()
{
	std::cout << BG_BLUE << "Server destructor called" << RESET << std::endl;
	for (unsigned long i = 0; i < _sockets.size(); i++)
		delete _sockets[i];
}

std::vector<Socket *> Server::getSockets() const
{
	return _sockets;
}

void Server::log(std::string const &message)
{
	std::cout << message << std::endl;
}

void Server::testConnection(int item)
{
	if (item < 0)
	{
		perror("Failed to connect...");
		_exit(1);
	}
}

void Server::printClientAddress(sockaddr_in const &address)
{
	char client_address[1024];
	inet_ntop(AF_INET, &address, client_address, 1024);
	std::cout << BG_GREEN << "Client connection: " << client_address << RESET << std::endl;
}

ServerConfig *Server::getConfig() const
{
	return _config;
}