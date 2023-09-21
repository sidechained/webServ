#pragma once
#include "../Sockets/SimpleSocket.hpp"
#include "../Colors.hpp"
#include <vector>

class SimpleServer
{
private:
    std::vector<SimpleSocket *>_sockets;
    void accepter();
    void handler();
    void responder();
protected:
    void log(std::string const &message);
    void printClientAddress(sockaddr_in const &address);
public:
    SimpleServer(int domain, int type, int protocol, std::vector<int> ports, std::string ip, int backlog);
    ~SimpleServer();
    void launch();
    std::vector<SimpleSocket*> getSockets() const;
    void testConnection(int item);
};
