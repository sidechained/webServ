#pragma once
#include "SimpleSocket.hpp"

class ListeningSocket : public SimpleSocket
{
private:
    int _backlog;
    int _listening;
public:
    ListeningSocket(int domain, int type, int protocol, int port, std::string ip, int backlog);
    ~ListeningSocket();
    void startListening();
};

