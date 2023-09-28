#include "ServerManager.hpp"

ServerManager::ServerManager() {}

ServerManager::ServerManager(ConfigFileParser *config) : _config(config) {}

ServerManager::~ServerManager()
{
	// delete _servers
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		delete *it;
	}

	// iterate and delete _pendingResponses
	for (std::map<int, SimpleResponse *>::iterator it = _pendingResponses.begin(); it != _pendingResponses.end(); ++it)
	{
		delete it->second;
	}
}

void ServerManager::setupServers()
{
	std::cout << std::endl;

	for (std::vector<ServerConfig>::iterator it = _config->serverConfigs.begin(); it != _config->serverConfigs.end(); ++it)
	{
		PRINT(SERVERMANAGER, BG_BOLD_BLUE, "Initializing server " << it - _config->serverConfigs.begin() + 1 << " of " << _config->serverConfigs.size() << RESET << "\n")
		_servers.push_back(new Server(*it));
	}
	PRINT(SERVERMANAGER, BG_BOLD_BLUE, "Servers initialized!" << RESET << "\n")
}

Socket *ServerManager::findSocket(int fd)
{
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		std::vector<Socket *> sockets = (*it)->getSockets();
		for (std::vector<Socket *>::iterator it2 = sockets.begin(); it2 != sockets.end(); ++it2)
		{
			int sock = (*it2)->getSock();
			if (sock == fd)
				return *it2;
		}
	}
	return NULL;
}

Server *ServerManager::findserver(int fd)
{
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		std::vector<Socket *> sockets = (*it)->getSockets();
		for (std::vector<Socket *>::iterator it2 = sockets.begin(); it2 != sockets.end(); ++it2)
		{
			int sock = (*it2)->getSock();
			if (sock == fd)
				return *it;
		}
	}
	return NULL;
}

void ServerManager::initializeSets()
{
	FD_ZERO(&_recv_fd_pool);
	FD_ZERO(&_write_fd_pool);

	// adds servers sockets to _recv_fd_pool set
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		std::vector<Socket *> sockets = (*it)->getSockets();

		for (std::vector<Socket *>::iterator it2 = sockets.begin(); it2 != sockets.end(); ++it2)
		{
			int sock = (*it2)->getSock();
			addToSet(sock, _recv_fd_pool);
			_servers_map.insert(std::make_pair(sock, *it));

			if (sock > _biggest_fd)
				_biggest_fd = sock;
		}
	}
	PRINT(SERVERMANAGER, BG_CYAN, "Sockets added to reciving fd set" << RESET << "\n")
}

void ServerManager::addToSet(const int i, fd_set &set)
{
	FD_SET(i, &set);
	if (i > _biggest_fd)
		_biggest_fd = i;
}

void ServerManager::removeFromSet(const int i, fd_set &set)
{
	FD_CLR(i, &set);
	if (i == _biggest_fd)
		_biggest_fd--;
}

void ServerManager::runServers()
{
	fd_set recv_set_cpy;
	fd_set write_set_cpy;
	int select_ret;

	_biggest_fd = 0;
	initializeSets();
	struct timeval timer;
	PRINT(SERVERMANAGER, BG_BOLD_BLUE, "Servers are running...  - select()" << RESET << "\n\n")
	while (true)
	{
		timer.tv_sec = 1;
		timer.tv_usec = 0;
		recv_set_cpy = _recv_fd_pool;
		write_set_cpy = _write_fd_pool;

		if ((select_ret = select(_biggest_fd + 1, &recv_set_cpy, &write_set_cpy, NULL, &timer)) < 0)
		{
			exit(1);
			continue;
		}
		for (int i = 0; i <= _biggest_fd; ++i)
		{
			if (FD_ISSET(i, &recv_set_cpy) && _servers_map.count(i))
				acceptNewConnection(i);
			else if (FD_ISSET(i, &recv_set_cpy) && _clients_map.count(i))
				readRequest(i, _clients_map[i]);
			else if (FD_ISSET(i, &write_set_cpy) && _clients_map.count(i))
			{
				sendResponse(i, _clients_map[i]);
			}
		}
		checkTimeout();
	}
}
void ServerManager::checkTimeout()
{
	for (std::map<int, Socket *>::iterator it = _clients_map.begin(); it != _clients_map.end(); ++it)
	{
		if (time(NULL) - (*it->second).getLastTime() > CONNECTION_TIMEOUT)
		{
			std::cout << BG_RED "client timeout " << time(NULL) - (*it->second).getLastTime() << RESET << std::endl;
			closeConnection(it->first);
			return;
		}
	}
}

void ServerManager::acceptNewConnection(int fd)
{
	struct sockaddr_in client_address;
	long client_address_size = sizeof(client_address);
	int client_sock;

	if ((client_sock = accept(fd, (struct sockaddr *)&client_address,
							  (socklen_t *)&client_address_size)) == -1)
	{
		std::cout << "accept error" << std::endl;
		return;
	}

	addToSet(client_sock, _recv_fd_pool);

	if (fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cout << "fcntl error" << std::endl;
		removeFromSet(client_sock, _recv_fd_pool);
		close(client_sock);
		return;
	}
	Socket *new_client = findSocket(fd);
	Server *server = findserver(fd);
	PRINT(SERVERMANAGER, BG_BOLD_BLUE, "CONNECTION accepted from client: " << new_client->getIp() << " : " << new_client->getPort())
	PRINT(SERVERMANAGER, CYAN, "\tFor server: " << new_client->getIp() << " on port: " << new_client->getPort() << " communication Socket creaed, in read mode waiting for a request. fd: " << client_sock)

	if (new_client)
	{
		_clients_map_server.insert(std::make_pair(client_sock, server));
		_clients_map.insert(std::make_pair(client_sock, new_client));
	}
}

Server *ServerManager::findServer(Socket *client)
{
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		std::vector<Socket *> sockets = (*it)->getSockets();
		for (std::vector<Socket *>::iterator it2 = sockets.begin(); it2 != sockets.end(); ++it2)
		{
			int sock = (*it2)->getSock();
			if (sock == client->getSock())
			{
				return *it;
			}
		}
	}
	return NULL;
}

void ServerManager::readRequest(const int &i, Socket *client)
{

	char buffer[MESSAGE_BUFFER];
	int bytes_read = 0;
	bytes_read = read(i, buffer, MESSAGE_BUFFER);
	if (bytes_read == 0)
	{
		closeConnection(i);
		return;
	}
	else if (bytes_read < 0)
	{
		std::cout << "read error" << std::endl;
		closeConnection(i);
		return;
	}
	else
	{
		PRINT(SERVERMANAGER, BG_BOLD_CYAN, "\tREQUEST from client: " << client->getIp() << " : " << client->getPort() << " has been read")
		Server *server = findServer(client);

		ServerConfig *config = server->getConfig();
		std::cout << "REQUEST:" << std::endl << buffer << std::endl;


/*	JUST TO PRINT OUT
	// Create a new file stream
    std::ofstream outputFile("output.txt");

    // Redirect std::cout to the output file
    std::streambuf* originalCoutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(outputFile.rdbuf());

    // Your code here
    std::cout << "REQUEST:" << std::endl << buffer << std::endl;

    // Restore std::cout to its original buffer
    std::cout.rdbuf(originalCoutBuffer);

    // Close the output file
    outputFile.close();*/

		HttpRequest parsedRequest(buffer, config);

		client->updateTime();
		_pendingResponses[i] = ResponseFactory::createResponse(parsedRequest);
		removeFromSet(i, _recv_fd_pool);
		addToSet(i, _write_fd_pool);
		PRINT(SERVERMANAGER, CYAN, "\tFor server: " << client->getIp() << " on port: " << client->getPort() << " communication Socket set to write mode for the response. fd: " << i)
	}
}

void ServerManager::sendResponse(const int &i, Socket *client)
{
	PRINT(SERVERMANAGER, BG_BOLD_CYAN, "\tRESPONSE to client: " << client->getIp() << " : " << client->getPort() << " has been written")

	int bytes_sent;

	SimpleResponse *responsePtr = _pendingResponses[i];
	std::string response = responsePtr->getResponse();

	if (response.length() >= MESSAGE_BUFFER)
		bytes_sent = write(i, response.c_str(), MESSAGE_BUFFER);
	else
		bytes_sent = write(i, response.c_str(), response.length());

	if (bytes_sent < 0)
	{
		std::cout << "Problem sending" << std::endl;
		closeConnection(i);
	}
	else if (bytes_sent == 0 || (size_t)bytes_sent == response.length())
	{
		PRINT(SERVERMANAGER, CYAN, "\tFor server: " << client->getIp() << " on port: " << client->getPort() << " bytes sent: " << bytes_sent << "  fd: " << i)
		_pendingResponses.erase(i);
		closeConnection(i);
	}
	else
	{
		client->updateTime();
		PRINT(SERVERMANAGER, BOLD_BLUE, "\t\tNOT entire data sent, bytes: " << bytes_sent << "  " << responsePtr->getResponse().size())

		responsePtr->cutRes(response, bytes_sent);
	}
}

void ServerManager::closeConnection(const int i)
{
	if (FD_ISSET(i, &_write_fd_pool))
		removeFromSet(i, _write_fd_pool);
	if (FD_ISSET(i, &_recv_fd_pool))
		removeFromSet(i, _recv_fd_pool);
	close(i);
	_clients_map.erase(i);
	PRINT(SERVERMANAGER, BG_BOLD_BLUE, "CONNECTION closed" << RESET << "\n\n")
}
