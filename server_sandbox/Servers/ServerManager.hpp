#pragma once
#include "SimpleServer.hpp"
#include "../Requests/HttpRequest.hpp"
#include "../Responses/TextResponse.hpp"
#include "../Responses/ResponseFactory.hpp"
#include "unistd.h"
#include <cstring>
#include <poll.h>
#include <map>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <csignal>
#include "../WebServ.hpp"

#define MAX_CLIENTS 10

class ServerManager
{
private:

	std::vector<SimpleServer *> _servers;
	std::map<int, SimpleServer*> _servers_map;
	std::map<int, ListeningSocket> _clients_map;
	fd_set     _recv_fd_pool;
	fd_set     _write_fd_pool;
	int        _biggest_fd;



	char _buffer[30000];
	struct pollfd _fds[MAX_CLIENTS + 1];
	int _clients;
	int _poll;
	std::map<int, SimpleResponse> _pendingResponses;
	void accepter();
	void handler();
	void responder();

public:
	void setupServers();
	void initializeSets();
	void addToSet(const int i, fd_set &set);
	void runServers();


	ServerManager();
	ServerManager(int domain, int type, int protocol, std::vector<int> ports, std::string ip, int backlog);
	~ServerManager();
	void launch();
};