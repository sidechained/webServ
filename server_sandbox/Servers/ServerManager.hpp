#pragma once
#include "Server.hpp"
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
#include <cstddef>
//#include "../Parser/ConfigFileParser.hpp"

#define MAX_CLIENTS 10
#define MESSAGE_BUFFER 20000
#define CONNECTION_TIMEOUT 60

class ServerManager
{
private:

	std::vector<Server *> _servers;
	std::map<int, Server*> _servers_map;
	std::map<int, Socket*> _clients_map;
	fd_set     _recv_fd_pool;
	fd_set     _write_fd_pool;
	int        _biggest_fd;

	//ConfigFileParser* _config;


	std::map<int, SimpleResponse *> _pendingResponses;


	Socket* findSocket(int fd);

public:
	void setupServers();
	void initializeSets();
	void addToSet(const int i, fd_set &set);
	void removeFromSet(const int i, fd_set &set);
	void runServers();
	void checkTimeout();
	void closeConnection(const int i);
	void acceptNewConnection(int fd);
	void readRequest(const int &i, Socket* client);
	void sendResponse(const int &i, Socket* client);


	ServerManager();
	~ServerManager();
	//void launch();
};