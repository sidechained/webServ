#include "ServerManager.hpp"

ServerManager::ServerManager(){}
ServerManager::~ServerManager(){
	// DELETE THE SERVERS!
}

void    ServerManager::setupServers()
{
    std::cout << std::endl;
    std::cout << BG_BOLD_CYAN << "Initializing servers..." << RESET << std::endl;

	std::vector<int> ports;
    ports.push_back(8002);
	//SimpleServer* server1 = new SimpleServer(AF_INET, SOCK_STREAM, 0, ports, "127.0.0.2", MAX_CLIENTS);
    //_servers.push_back(server1);
	std::cout << "server 1 not created" << std::endl;
    _servers.push_back(new SimpleServer(AF_INET, SOCK_STREAM, 0, ports, "127.0.0.1", MAX_CLIENTS));
	std::cout << "server 1 created" << std::endl;
    //_servers.push_back(new SimpleServer(AF_INET, SOCK_STREAM, 0, ports, "127.0.0.3", MAX_CLIENTS));

}

ListeningSocket* ServerManager::findSocket(int fd)
{
	for(std::vector<SimpleServer *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
		std::vector<ListeningSocket *> sockets = (*it)->getSockets();
		for(std::vector<ListeningSocket *>::iterator it2 = sockets.begin(); it2 != sockets.end(); ++it2)
		{
			int sock = (*it2)->getSock();
			if (sock == fd)
				return *it2;
		}
    }
	return NULL;
}

void    ServerManager::initializeSets()
{
    FD_ZERO(&_recv_fd_pool);
    FD_ZERO(&_write_fd_pool);

    // adds servers sockets to _recv_fd_pool set
    for(std::vector<SimpleServer *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
		std::vector<ListeningSocket *> sockets = (*it)->getSockets();
		for(std::vector<ListeningSocket *>::iterator it2 = sockets.begin(); it2 != sockets.end(); ++it2)
		{
			int sock = (*it2)->getSock();
			if (listen(sock, MAX_CLIENTS) == -1)
			{
				//Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: listen error: %s   Closing....", strerror(errno));
				exit(EXIT_FAILURE);
			}
			if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
			{
				//Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: fcntl error: %s   Closing....", strerror(errno));
				exit(EXIT_FAILURE);
			}
			addToSet(sock, _recv_fd_pool);
			_servers_map.insert(std::make_pair(sock, *it));

			std::cout << "sock n: " << sock << " added to server" << std::endl;
			
			if (sock > _biggest_fd)
				_biggest_fd = sock;
		}
    }
	std::cout << BG_CYAN "server manager initialized" RESET << std::endl;
}

void	ServerManager::addToSet(const int i, fd_set &set)
{
    FD_SET(i, &set);
    if (i > _biggest_fd)
        _biggest_fd = i;
}

void	ServerManager::removeFromSet(const int i, fd_set &set)
{
    FD_CLR(i, &set);
    if (i == _biggest_fd)
        _biggest_fd--;
}

void    ServerManager::runServers()
{
	std::cout << "runservers function" << std::endl;
    fd_set  recv_set_cpy;
    fd_set  write_set_cpy;
    int     select_ret;

    _biggest_fd = 0;
    initializeSets();
    struct timeval timer;
    while (true)
    {
		//std::cout << "infinite loop" << std::endl;
        timer.tv_sec = 1;
        timer.tv_usec = 0;
        recv_set_cpy = _recv_fd_pool;
        write_set_cpy = _write_fd_pool;

        if ( (select_ret = select(_biggest_fd + 1, &recv_set_cpy, &write_set_cpy, NULL, &timer)) < 0 )
        {
		    //Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: select error %s   Closing ....", strerror(errno));
			std::cout << "select error!" << std::endl;
            exit(1);
            continue ;
        }
        for (int i = 0; i <= _biggest_fd; ++i)
        {
            if (FD_ISSET(i, &recv_set_cpy) && _servers_map.count(i))
				acceptNewConnection(i);
                //acceptNewConnection(_servers_map.find(i)->second);
            else if (FD_ISSET(i, &recv_set_cpy) && _clients_map.count(i))
                readRequest(i);
            else if (FD_ISSET(i, &write_set_cpy) && _clients_map.count(i))
            {
				sendResponse(i);
				//closeConnection(i);
				std::cout << "response sent" << std::endl;
                /*int cgi_state = _clients_map[i].response.getCgiState(); // 0->NoCGI 1->CGI write/read to/from script 2-CGI read/write done
                if (cgi_state == 1 && FD_ISSET(_clients_map[i].response._cgi_obj.pipe_in[1], &write_set_cpy))
                    sendCgiBody(_clients_map[i], _clients_map[i].response._cgi_obj);
                else if (cgi_state == 1 && FD_ISSET(_clients_map[i].response._cgi_obj.pipe_out[0], &recv_set_cpy))
                    readCgiResponse(_clients_map[i], _clients_map[i].response._cgi_obj);
                else if ((cgi_state == 0 || cgi_state == 2)  && FD_ISSET(i, &write_set_cpy))
                    sendResponse(i, _clients_map[i]);*/
            }
        }
        //checkTimeout();
    }
}

void    ServerManager::acceptNewConnection(int fd)
{
	std::cout << "accept new connection function" << std::endl;
    struct sockaddr_in client_address;
    long  client_address_size = sizeof(client_address);
    int client_sock;
    //ListeningSocket  new_client(serv);
    char    buf[INET_ADDRSTRLEN];

    if ( (client_sock = accept(fd, (struct sockaddr *)&client_address,
     (socklen_t*)&client_address_size)) == -1)
    {
        std::cout << "accept error" << std::endl;
        return ;
    }
	std::cout << BG_BOLD_CYAN << "connection from " << inet_ntop(AF_INET, &client_address, buf, INET_ADDRSTRLEN) << "assigned socket " << client_sock << RESET << std::endl;
    //Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "New Connection From %s, Assigned Socket %d",inet_ntop(AF_INET, &client_address, buf, INET_ADDRSTRLEN), client_sock);

    addToSet(client_sock, _recv_fd_pool);

    if (fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
    {
        //Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: fcntl error %s", strerror(errno));
        removeFromSet(client_sock, _recv_fd_pool);
        close(client_sock);
        return ;
    }
	ListeningSocket* new_client = findSocket(fd);
	std::cout << "socket found" << new_client << std::endl;
	if (new_client)
	{
		//std::cout << "adding client to map" << std::endl;
		_clients_map.insert(std::make_pair(client_sock, new_client));
		std::cout << "client added to map, socket: " << client_sock << fd << _biggest_fd << std::endl;

	}

	// ADDING THE SOCKETS TO THE coresponding MAP
    /*new_client.setSocket(client_sock);
    if (_clients_map.count(client_sock) != 0)
        _clients_map.erase(client_sock);
    _clients_map.insert(std::make_pair(client_sock, new_client));*/
}

/*void PollingServer::handler()
{
	for (int i = 1; i <= _clients; i++)
	{
		if (_fds[i].revents & POLLIN)
		{
			memset(_buffer, 0, sizeof(_buffer));
			int bytesRead = read(_fds[i].fd, _buffer, sizeof(_buffer));
			if (bytesRead > 0)
			{
				std::cout << BG_GREEN << "Client " << i << " sent a request" << RESET << std::endl;
				HttpRequest parsedRequest(_buffer);
				parsedRequest.printRequest();
				_pendingResponses[_fds[i].fd] = TextResponse(parsedRequest);
			}
			else
			{
				std::cout << BG_GREEN << "Client " << i << " disconnected" << RESET << std::endl;
				close(_fds[i].fd);
				if (i < _clients)
					_fds[i] = _fds[_clients];
				_clients--;
			}
		}
	}
}*/

void    ServerManager::readRequest(const int &i)
{
	//(void) c;
	std::cout << "read request" << std::endl;


    char    buffer[MESSAGE_BUFFER];
    int     bytes_read = 0;
    bytes_read = read(i, buffer, MESSAGE_BUFFER);
    if (bytes_read <= 0)
    {
        //Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "webserv: Client %d Closed Connection", i);
        closeConnection(i);
        return ;
    }
	else
	{
		std::cout << BG_GREEN << "Client " << i << " sent a request" << RESET << std::endl;
		HttpRequest parsedRequest(buffer);
		std::cout << BG_BOLD_WHITE << buffer << RESET << std::endl;
		parsedRequest.printRequest();
		_pendingResponses[i] = TextResponse(parsedRequest);
		removeFromSet(i, _recv_fd_pool);
		addToSet(i, _write_fd_pool);
	}
    /*else if (bytes_read < 0)
    {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: fd %d read error %s", i, strerror(errno));
        closeConnection(i);
        return ;
    }
    else if (bytes_read != 0)
    {
        c.updateTime();
        c.request.feed(buffer, bytes_read);
        memset(buffer, 0, sizeof(buffer));
    }

    if (c.request.parsingCompleted() || c.request.errorCode()) // 1 = parsing completed and we can work on the response.
    {
        assignServer(c);
        Logger::logMsg(CYAN, CONSOLE_OUTPUT, "Request Recived From Socket %d, Method=<%s>  URI=<%s>"
        , i, c.request.getMethodStr().c_str(), c.request.getPath().c_str());
        c.buildResponse();
        if (c.response.getCgiState())
        {
            handleReqBody(c);
            addToSet(c.response._cgi_obj.pipe_in[1],  _write_fd_pool);
            addToSet(c.response._cgi_obj.pipe_out[0],  _recv_fd_pool);
        }
        removeFromSet(i, _recv_fd_pool);
        addToSet(i, _write_fd_pool);
    }*/
}

/*void PollingServer::responder()
{
	for (int i = 1; i <= _clients; i++)
	{
		if (_fds[i].revents & POLLOUT)
		{
			int clientFd = _fds[i].fd;

			SimpleResponse response = _pendingResponses[clientFd];

			if (!response.isHeaderSent())
			{
				std::string header = response.getHeader();
				ssize_t bytes_sent = write(clientFd, header.c_str(), header.length());
				if (bytes_sent == -1)
				{
					std::cerr << "Error sending HTTP response headers" << std::endl;
					return;
				}
				if ((unsigned long)bytes_sent == header.length())
					response.setHeaderSent(true);
				else
					response.updateHeaderOffset(bytes_sent);
			}

			if (!response.isBodySent())
			{
				std::string body = response.getBody();
				ssize_t bytes_sent = write(clientFd, body.c_str(), body.length());
				if (bytes_sent == -1)
				{
					std::cerr << "Error sending HTTP response body" << std::endl;
					return;
				}
				if ((unsigned long)bytes_sent == body.length())
					response.setBodySent(true);
				else
					response.updateBodyOffset(bytes_sent);
			}
			// Remove the response from the map if it has been sent
			if (response.isHeaderSent() && response.isBodySent())
				_pendingResponses.erase(clientFd);
		}
	}
}*/

void    ServerManager::sendResponse(const int &i)
{
	std::cout << BG_GREEN "sendResponse call fd: " RESET << i << std::endl;

	int bytes_sent;

	SimpleResponse responsePtr = _pendingResponses[i];
    std::string response = responsePtr.getResponse();
	std::cout << BG_BOLD_MAGENTA << response << RESET << std::endl;
    if (response.length() >= MESSAGE_BUFFER)
        bytes_sent = write(i, response.c_str(), MESSAGE_BUFFER);
    else
        bytes_sent = write(i, response.c_str(), response.length());

    if (bytes_sent < 0)
    {
        std::cout << "Problem sending" << std::endl;
        closeConnection(i);
    }
    else if (bytes_sent == 0 ||(size_t) bytes_sent == response.length())
    {
		_pendingResponses.erase(i);
		closeConnection(i);
		std::cout << "connection closed" << std::endl;
    }
	else
	{
		responsePtr.cutRes(bytes_sent);
		std::cout << BG_BOLD_RED "NOT entire data sent" RESET << std::endl;
	}



    /*int bytes_sent;
    std::string response = c.response.getRes();
    if (response.length() >= MESSAGE_BUFFER)
        bytes_sent = write(i, response.c_str(), MESSAGE_BUFFER);
    else
        bytes_sent = write(i, response.c_str(), response.length());

    if (bytes_sent < 0)
    {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "sendResponse(): error sending : %s", strerror(errno));
        closeConnection(i);
    }
    else if (bytes_sent == 0 || (size_t) bytes_sent == response.length())
    {
        // Logger::logMsg(LIGHTMAGENTA, CONSOLE_OUTPUT, "sendResponse() Done sending ");
        Logger::logMsg(CYAN, CONSOLE_OUTPUT, "Response Sent To Socket %d, Stats=<%d>"
        , i, c.response.getCode());
        if (c.request.keepAlive() == false || c.request.errorCode() || c.response.getCgiState())
        {
            Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "Client %d: Connection Closed.", i);
            closeConnection(i);
        }
        else
        {
            removeFromSet(i, _write_fd_pool);
            addToSet(i, _recv_fd_pool);
            c.clearClient();
        }
    }
    else
    {
        c.updateTime();
        c.response.cutRes(bytes_sent);
    }*/
}

void    ServerManager::closeConnection(const int i)
{
    if (FD_ISSET(i, &_write_fd_pool))
        removeFromSet(i, _write_fd_pool);
    if (FD_ISSET(i, &_recv_fd_pool))
        removeFromSet(i, _recv_fd_pool);
    close(i);
    _clients_map.erase(i);
}

/*void    ServerManager::checkTimeout()
{
    for(std::map<int, ListeningSocket>::iterator it = _clients_map.begin() ; it != _clients_map.end(); ++it)
    {
        if (time(NULL) - it->second.getLastTime() > CONNECTION_TIMEOUT)
        {
            //Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "Client %d Timeout, Closing Connection..", it->first);
            closeConnection(it->first);
            return ;
        }
    }
}*/