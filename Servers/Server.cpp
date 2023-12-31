#include "Server.hpp"

Server::Server(ServerConfig &config) : _config(&config), _ports(config.portConfigs), _ip(config.ip)
{
    for (unsigned long i = 0; i < config.portConfigs.size(); i++)
    {
		Socket *socket = new Socket(AF_INET, SOCK_STREAM, 0, _ports[i].number, _ip, MAX_CLIENTS);
		if (!socket->createSocket())
			delete socket;
		else
			_sockets.push_back(socket);
    }
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