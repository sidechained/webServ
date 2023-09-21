#pragma once
#include "../Sockets/ListeningSocket.hpp"
#include "../Parser/ServerConfig.hpp"
#include "../Colors.hpp"
#include <vector>

class SimpleServer
{
private:
    std::vector<ListeningSocket *>_sockets;
    void accepter();
    void handler();
    void responder();
protected:
	ServerConfig *_config;
    void log(std::string const &message);
    void printClientAddress(sockaddr_in const &address);
public:
	SimpleServer(ServerConfig &config);
    SimpleServer(int domain, int type, int protocol, std::vector<int> ports, std::string ip, int backlog);
    ~SimpleServer();
    void launch();
    std::vector<ListeningSocket*> getSockets() const;
    void testConnection(int item);
};
