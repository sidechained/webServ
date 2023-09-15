#include "SimpleServer.hpp"


SimpleServer::SimpleServer(int domain, int type, int protocol, int port, std::string ip, int backlog)
{
    _socket = new ListeningSocket(domain, type, protocol, port, ip, backlog);
}

SimpleServer::~SimpleServer()
{
    std::cout << BG_BLUE << "SimpleServer destructor called" << RESET << std::endl;
    delete _socket;
}

ListeningSocket *SimpleServer::getSocket() const
{
    return _socket;
}

void SimpleServer::log(std::string const &message)
{
    std::cout << message << std::endl;
}

void SimpleServer::testConnection(int item)
{
    if (item < 0)
    {
        perror("Failed to connect...");
        _exit(1);
    }
}

void SimpleServer::printClientAddress(sockaddr_in const &address)
{
    char client_address[1024];
    inet_ntop(AF_INET, &address, client_address, 1024);
    std::cout << BG_GREEN << "Client connection: " << client_address << RESET << std::endl;
}