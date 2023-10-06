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
				int cgi = _pendingResponses[i]->isCgi();
				if (cgi)
				{
					FormResponse *cgiResponse = dynamic_cast<FormResponse *>(_pendingResponses[i]);
					if (FD_ISSET(cgiResponse->input_pipefd[1], &write_set_cpy))
						sendBodyToCgi(cgiResponse, _clients_map[i]);
					else if (FD_ISSET(cgiResponse->output_pipefd[0], &recv_set_cpy))
						readBodyFromCgi(cgiResponse);
				}
				else
					sendResponse(i, _clients_map[i]);
			}
		}
		checkTimeout();
	}
}

void ServerManager::sendBodyToCgi(FormResponse *cgiResponse, Socket *client)
{
	std::vector<char> body = cgiResponse->getRequest().getBodyVector();
	// Check if the pipe is valid
	if (cgiResponse->input_pipefd[1] != -1)
	{
		char EOF_char = EOF;
		std::vector<char> buffer(body.begin(), body.end());
		buffer.push_back(EOF_char);

		ssize_t bytes_written = write(cgiResponse->input_pipefd[1], buffer.data(), buffer.size());

		PRINT(CGI, BG_BOLD_CYAN, "\t\tSending body to cgi. Size is: " << body.size())

		if (bytes_written == -1)
		{
			perror("write");
			close(cgiResponse->input_pipefd[1]);
			removeFromSet(cgiResponse->input_pipefd[1], _write_fd_pool);
		}
		else if (bytes_written == 0)
		{
			close(cgiResponse->input_pipefd[1]);
			removeFromSet(cgiResponse->input_pipefd[1], _write_fd_pool);
		}
		else
		{
			close(cgiResponse->input_pipefd[1]);
			removeFromSet(cgiResponse->input_pipefd[1], _write_fd_pool);
		}
	}
	else
	{
		perror("invalid pipe");
		closeConnection(cgiResponse->input_pipefd[1]);
	}

	removeFromSet(cgiResponse->input_pipefd[1], _write_fd_pool);

	/* TIMEOUT CHECK CGI */

	time_t startTime = time(NULL);
	int status;
	bool killed = false; // Flag to track if the child process has been killed

	while (!killed)
	{
		// Check if the child process has terminated
		pid_t result = waitpid(cgiResponse->child_pid, &status, WNOHANG);

		if (result == 0)
		{
			// Child process is still running
			time_t currentTime = time(NULL);
			int elapsedSeconds = static_cast<int>(currentTime - startTime);

			if (elapsedSeconds >= 6)
			{
				// Timeout exceeded, kill the child process
				PRINT(CGI, BG_RED, "\t\tTimeout exceeded, killing child process")
				kill(cgiResponse->child_pid, SIGKILL);
				killed = true;	// Set the flag to indicate that the child process has been killed

				std::string error = "501";
				Server *server = findServer(client);
				ServerConfig *config = server->getConfig();
				cgiResponse->createErrResponse(error, config);
				return;
			}
			else
			{
				// Sleep for a short time before checking again
				sleep(1);
			}
		}
		else if (result == -1)
		{
			// Error occurred, handle it as needed
			perror("waitpid");
			break;
		}
		else
		{
			if (WIFEXITED(status))
			{
				int exit_status = WEXITSTATUS(status);
				PRINT(CGI, BG_GREEN, "\t\tChild process exited with status: " << exit_status)
				if (exit_status != 0)
				{
					std::string error = "501";
					Server *server = findServer(client);
					ServerConfig *config = server->getConfig();
					cgiResponse->createErrResponse(error, config);
					return;
				}
			}
			// Child process has terminated
			break;
		}
	}
}

void ServerManager::readBodyFromCgi(FormResponse *cgiResponse)
{

	cgiResponse->setCgi(false);
	char buffer[MESSAGE_BUFFER];
	bzero(buffer, MESSAGE_BUFFER);
	int bytes_read = 0;
	bytes_read = read(cgiResponse->output_pipefd[0], buffer, MESSAGE_BUFFER);
	PRINT(CGI, BG_BOLD_CYAN, "\t\tRead from cgi. Bytes read: " << bytes_read)
	if (bytes_read < 0)
	{
		perror("read");
		close(cgiResponse->output_pipefd[0]);
		removeFromSet(cgiResponse->output_pipefd[0], _recv_fd_pool);
		return;
	}
	else if (bytes_read == 0)
	{
		close(cgiResponse->output_pipefd[0]);
		removeFromSet(cgiResponse->output_pipefd[0], _recv_fd_pool);
		return;
	}

	// converting the buffer to a string
	std::vector<char> bufferVector(buffer, buffer + bytes_read);
	close(cgiResponse->output_pipefd[0]);
	std::string htmlString(bufferVector.begin(), bufferVector.end());

	// if the string is not empty, set the body and the header
	if (!htmlString.empty())
	{
		cgiResponse->setBody(htmlString);
		cgiResponse->setHeader(OkHeader("text/html", cgiResponse->getBodyLength()).getHeader());
	}
	removeFromSet(cgiResponse->output_pipefd[0], _recv_fd_pool);
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

	// for non blocking
	if (fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cout << "fcntl error" << std::endl;
		removeFromSet(client_sock, _recv_fd_pool);
		close(client_sock);
		return;
	}
	Socket *new_client = findSocket(fd);
	new_client->updateTime();
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

	char buffer[REQUEST_BUFFER];
	bzero(buffer, REQUEST_BUFFER);
	int bytes_read = 0;

	bytes_read = read(i, buffer, REQUEST_BUFFER);
	std::vector<char> bufferVector;
	for (int i = 0; i < bytes_read; i++)
		bufferVector.push_back(buffer[i]);
	PRINT(SERVERMANAGER, BG_BOLD_CYAN, "\tREQUEST read bytes:" << bytes_read << " from client: " << client->getIp() << " : " << client->getPort())

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
		HttpRequest parsedRequest(buffer, bufferVector, config);
		PRINT(CGI, BG_BOLD_CYAN, "\tReading request with method: " << parsedRequest.getMethod() << ", and file type: " << parsedRequest.getContentType())
		client->updateTime();

		// creating a response object
		_pendingResponses[i] = ResponseFactory::createResponse(parsedRequest);
		if (_pendingResponses[i]->isCgi())
		{
			FormResponse *c = dynamic_cast<FormResponse *>(_pendingResponses[i]);
			addToSet(c->getInputPipefd(), _write_fd_pool);
			addToSet(c->getOutputPipefd(), _recv_fd_pool);
			c->createResponse(parsedRequest);
		}

		// setting the socket to write mode
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
	if (!responsePtr)
	{
		std::cout << "responsePtr is NULL" << std::endl;
		closeConnection(i);
		return;
	}
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
		// PRINT(SERVERMANAGER, CYAN, "\tFor server: " << client->getIp() << " on port: " << client->getPort() << " bytes sent: " << bytes_sent << "  fd: " << i)
		// PRINT(SERVERMANAGER, BG_BOLD_CYAN, "\tRESPONSE length: " << response.length() << " bytes sent: " << bytes_sent << "  fd: " << i)
		delete _pendingResponses[i];
		_pendingResponses.erase(i);
		closeConnection(i);
	}
	else
	{
		client->updateTime();
		// PRINT(SERVERMANAGER, BOLD_BLUE, "\t\tNOT entire data sent, bytes: " << bytes_sent << "  " << responsePtr->getResponse().size())

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
