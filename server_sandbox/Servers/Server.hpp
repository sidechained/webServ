#pragma once
#include "../Sockets/Socket.hpp"
#include "../Colors.hpp"
#include <vector>

class Server
{
private:
    std::vector<Socket *>_sockets;
    void accepter();
    void handler();
    void responder();
protected:
    void log(std::string const &message);
    void printClientAddress(sockaddr_in const &address);
public:
    Server(int domain, int type, int protocol, std::vector<int> ports, std::string ip, int backlog);
    ~Server();
    void launch();
    std::vector<Socket*> getSockets() const;
    void testConnection(int item);
};
