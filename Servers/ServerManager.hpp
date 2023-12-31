#pragma once
#include "../Requests/HttpRequest.hpp"
#include "../Responses/SimpleResponse.hpp"
#include "Server.hpp"
#include "unistd.h"
#include <cstring>
#include <poll.h>
#include <map>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <csignal>
#include <cstddef>
#include "../Parser/ConfigFileParser.hpp"
#include "../Responses/ResponseFactory.hpp"
#include <time.h>
#include <stdio.h>
#include <ctime>

//#define MAX_CLIENTS 10
#define REQUEST_BUFFER 30000
#define MESSAGE_BUFFER 20000
#define CONNECTION_TIMEOUT 600

class Server;
class SimpleResponse;

class ServerManager
{
private:
	std::vector<Server *> _servers;
	std::map<int, Server *> _servers_map;
	std::map<int, Socket *> _clients_map;
	std::map<int, Server *> _clients_map_server;
	fd_set _recv_fd_pool;
	fd_set _write_fd_pool;
	int _biggest_fd;

	ConfigFileParser* _config;

	std::map<int, SimpleResponse *> _pendingResponses;

	Socket *findSocket(int fd);
	Server *findserver(int fd);
	Server* findServer(Socket* client);

public:
	ServerManager(ConfigFileParser *config);
	void setupServers();
	void initializeSets();
	void addToSet(const int i, fd_set &set);
	void removeFromSet(const int i, fd_set &set);
	void runServers();
	void checkTimeout();
	void closeConnection(const int i);
	void acceptNewConnection(int fd);
	void readRequest(const int &i, Socket *client);
	void sendResponse(const int &i, Socket *client);
	void sendBodyToCgi(FormResponse *cgiResponse, Socket *client);
	void readBodyFromCgi(FormResponse *cgiResponse);

	ServerManager();
	~ServerManager();
	// void launch();
};