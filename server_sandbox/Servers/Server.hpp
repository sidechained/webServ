#pragma once
#include "../Sockets/Socket.hpp"
#include "../Parser/ServerConfig.hpp"
#include "../Colors.hpp"
#include <vector>

#define MAX_CLIENTS 10


class Server
{
private:
	std::vector<Socket *> _sockets;
	void accepter();
	void handler();
	void responder();

protected:
	void log(std::string const &message);
	void printClientAddress(sockaddr_in const &address);
	ServerConfig *_config;
	std::vector<Port> _ports;
	std::string _ip;
public:
	//Server(int domain, int type, int protocol, std::vector<int> ports, std::string ip, int backlog);
	Server(ServerConfig &config);
	~Server();
	void launch();
	std::vector<Socket *> getSockets() const;
	void testConnection(int item);
	ServerConfig *getConfig() const;
};
