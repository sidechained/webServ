#pragma once
#include "SimpleServer.hpp"
#include "../Requests/HttpRequest.hpp"
#include "../Responses/TextResponse.hpp"
#include "../Responses/ResponseFactory.hpp"
#include "unistd.h"
#include <cstring>
#include <poll.h>
#include <map>
#include <fstream>
#include <csignal>
#include "../WebServ.hpp"

#define MAX_CLIENTS 10

class PollingServer : public SimpleServer
{
private:
    char _buffer[30000];
    struct pollfd _fds[MAX_CLIENTS + 1];
    int _clients;
    int _poll;
    std::map<int, SimpleResponse> _pendingResponses;
    void accepter();
    void handler();
    void responder();

public:
    PollingServer(int domain, int type, int protocol, int port, std::string ip, int backlog);
    ~PollingServer();
    void launch();
};
