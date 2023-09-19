#pragma once
#include "../Sockets/ListeningSocket.hpp"
#include "../Parser/ServerConfig.hpp"
#include "../Colors.hpp"
#include <vector>

class SimpleServer
{
private:
    std::vector<ListeningSocket *>_sockets;
    virtual void accepter() = 0;
    virtual void handler() = 0;
    virtual void responder() = 0;
protected:
    ServerConfig *_config;
    void log(std::string const &message);
    void printClientAddress(sockaddr_in const &address);
public:
    SimpleServer(int domain, int type, int protocol, std::vector<int> ports, std::string ip, int backlog);
    SimpleServer(ServerConfig &config);
    ~SimpleServer();
    virtual void launch() = 0;
    std::vector<ListeningSocket *> getSockets() const;
    void testConnection(int item);
};
