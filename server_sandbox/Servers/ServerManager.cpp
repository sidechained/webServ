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
				//SimpleResponse *responsePtr = _pendingResponses[i];
				int cgi = _pendingResponses[i]->isCgi();
				if (cgi)
				{
					//PRINT(CGI, BG_BLUE, "cgi form response text")
					FormResponse* cgiResponse = dynamic_cast<FormResponse*>(_pendingResponses[i]);
					if (FD_ISSET(cgiResponse->input_pipefd[1], &write_set_cpy))
						sendBodyToCgi(cgiResponse);
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

void ServerManager::sendBodyToCgi(FormResponse *cgiResponse)
{
	PRINT(CGI, BG_BLUE, "sendBodyToCgi")
	std::vector<char> body = cgiResponse->getRequest().getBodyVector();
	//std::string bodyStr(body.begin(), body.end());
	//std::cout << "body is" << bodyStr << std:: endl;
        // Check if the pipe is valid
    if (cgiResponse->input_pipefd[1] != -1) {
        // Write the entire body to the input pipe at once
        ssize_t bytes_written = write(cgiResponse->input_pipefd[1], body.data(), body.size());
		char EOF_char = EOF;
		write(cgiResponse->input_pipefd[1], &EOF_char, 1);
	PRINT(CGI, BG_YELLOW, "body size sent to cgi is: " << body.size() << " bytes written: " << bytes_written)

        if (bytes_written == -1) {
            // Handle write error
            perror("write");
			PRINT(CGI, BG_BOLD_RED, "write error")
            // Add error handling as needed
        }

        close(cgiResponse->input_pipefd[1]);
        removeFromSet(cgiResponse->input_pipefd[1], _write_fd_pool);
    } else {
		PRINT(CGI, BG_BOLD_RED, "invalid pipe")
        // Handle invalid pipe
        // Add error handling as needed
    }

		removeFromSet(cgiResponse->input_pipefd[1], _write_fd_pool);
}

void ServerManager::readBodyFromCgi(FormResponse *cgiResponse)
{
	PRINT(CGI, BG_BLUE, "readBodyFromCgi")

	std::vector<char> htmlOutputBuffer;  // Dynamic buffer to store the HTML output

	// Read the output from the child process and store it in htmlOutputBuffer
	char output_buffer[256];
	ssize_t output_bytes_read;
	while ((output_bytes_read = read(cgiResponse->output_pipefd[0], output_buffer, sizeof(output_buffer))) > 0) {
		htmlOutputBuffer.insert(htmlOutputBuffer.end(), output_buffer, output_buffer + output_bytes_read);
	}

	PRINT(CGI, BG_BLUE, "body size read from cgi is: " << htmlOutputBuffer.size())
	close(cgiResponse->output_pipefd[0]);

	// Convert the vector to a string
	std::string htmlString(htmlOutputBuffer.begin(), htmlOutputBuffer.end());
	// std::cout << "htmlString: " << htmlString << std::endl;

	/*char    buffer[MESSAGE_BUFFER * 2];
    int     bytes_read = 0;
    bytes_read = read(cgiResponse->output_pipefd[0], buffer, MESSAGE_BUFFER* 2);

	close(cgiResponse->output_pipefd[0]);

	std::string htmlString(buffer, bytes_read);*/
	

	cgiResponse->setBody(htmlString);
	//_body = htmlString;

	cgiResponse->setHeader(OkHeader(cgiResponse->getRequest().getContentType(), cgiResponse->getBodyLength()).getHeader());
	removeFromSet(cgiResponse->output_pipefd[0], _recv_fd_pool);

	int status;
	waitpid(cgiResponse->child_pid, &status, 0);

	if (WIFEXITED(status)) {
		std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
	}
	cgiResponse->setCgi(false);

	//std::cout << BG_BLUE << cgiResponse->getResponse() << RESET << std::endl;
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
		new_client->updateTime();
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
	std::cout << "request is " << buffer << std::endl;
	std::vector<char> bufferVector;
	for (int i = 0; i < bytes_read; i++)
		bufferVector.push_back(buffer[i]);
	//std::string bufferString(bufferVector.begin(), bufferVector.end());
	PRINT(SERVERMANAGER, BG_BOLD_CYAN, "\tREQUEST read bytes:"  << " from client: " << client->getIp() << " : " << client->getPort())
	// bufferVector.push_back('\0');
	//print bufferVector
	

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
		//std::cout << "REQUEST:" << std::endl << buffer << std::endl;
		HttpRequest parsedRequest(buffer, bufferVector, config);
		PRINT(CGI, BG_RED, "content length request: " << parsedRequest.getContentLength())
		// std::cout << "Printing request" << std::endl;
  	 	// parsedRequest.printRequest();
		std::cout << BG_GREEN << parsedRequest.getMethod() << std::endl;
		client->updateTime();
		_pendingResponses[i] = ResponseFactory::createResponse(parsedRequest);
		if (_pendingResponses[i]->isCgi())
        {
			std::cout << BG_BLUE "cgi " RESET << _pendingResponses[i]->isCgi() << std::endl;
			FormResponse* c = dynamic_cast<FormResponse*>(_pendingResponses[i]);
            addToSet(c->getInputPipefd(), _write_fd_pool);
            addToSet(c->getOutputPipefd(), _recv_fd_pool);
			c->createResponse(parsedRequest);
			PRINT(CGI, BG_BLUE, "cgi created from read request")
		}
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
	std::cout << "Response is: " << response << std::endl;
	//PRINT(SERVERMANAGER, CYAN, "response is: " << response << "response")

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
		PRINT(SERVERMANAGER, BG_BOLD_CYAN, "\tRESPONSE length: " << response.length() << " bytes sent: " << bytes_sent << "  fd: " << i)
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
