#pragma once
#include "../Sockets/ListeningSocket.hpp"
#include "../Colors.hpp"

class SimpleServer
{
private:
    ListeningSocket *_socket;
    virtual void accepter() = 0;
    virtual void handler() = 0;
    virtual void responder() = 0;
protected:
    void log(std::string const &message);
    void printClientAddress(sockaddr_in const &address);
public:
    SimpleServer(int domain, int type, int protocol, int port, std::string ip, int backlog);
    ~SimpleServer();
    virtual void launch() = 0;
    ListeningSocket *getSocket() const;
    void testConnection(int item);
};
