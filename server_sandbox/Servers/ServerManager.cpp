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
    ports.push_back(8003);
	std::vector<int> ports2;
    ports2.push_back(8004);
	//SimpleServer* server1 = new SimpleServer(AF_INET, SOCK_STREAM, 0, ports, "127.0.0.2", MAX_CLIENTS);
    //_servers.push_back(server1);
	std::cout << "server 1 not created" << std::endl;
    _servers.push_back(new SimpleServer(AF_INET, SOCK_STREAM, 0, ports, "127.0.0.1", MAX_CLIENTS));
    _servers.push_back(new SimpleServer(AF_INET, SOCK_STREAM, 0, ports2, "127.0.0.3", MAX_CLIENTS));
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
			//(*it2)->updateTime();
			int sock = (*it2)->getSock();
			if (listen(sock, MAX_CLIENTS) == -1)
			{
				std::cerr << "listen error" << std::endl;
				exit(EXIT_FAILURE);
			}
			if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
			{
				std::cerr << "fcntl error" << std::endl;
				exit(EXIT_FAILURE);
			}
			addToSet(sock, _recv_fd_pool);
			_servers_map.insert(std::make_pair(sock, *it));

			std::cout << BG_BOLD_BLUE "sock n: " << sock << " added to server" RESET << std::endl; // add server name
			
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
	//std::cout << "runservers function" << std::endl;
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
                readRequest(i, _clients_map[i]);
            else if (FD_ISSET(i, &write_set_cpy) && _clients_map.count(i))
            {
				sendResponse(i, _clients_map[i]);
				std::cout << "response sent" << std::endl;

            }
        }
		//std::cout << "check timeout" << std::endl;
        checkTimeout();
    }
}
void    ServerManager::checkTimeout()
{
    for(std::map<int, ListeningSocket*>::iterator it = _clients_map.begin() ; it != _clients_map.end(); ++it)
	{
		std::cout << "checking timeout" << std::endl;
    
        if (time(NULL) - (*it->second).getLastTime() > CONNECTION_TIMEOUT)
        {
			std::cout << BG_RED "client timeout " << time(NULL) - (*it->second).getLastTime() << RESET << std::endl;
            //Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "Client %d Timeout, Closing Connection..", it->first);
            closeConnection(it->first);
            return ;
        }
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

    addToSet(client_sock, _recv_fd_pool);

    if (fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
    {
		std::cout << "fcntl error" << std::endl;
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
}

void    ServerManager::readRequest(const int &i, ListeningSocket* client)
{
	std::cout << "read request" << std::endl;

    char    buffer[MESSAGE_BUFFER];
    int     bytes_read = 0;
    bytes_read = read(i, buffer, MESSAGE_BUFFER);
    if (bytes_read <= 0)
    {
		std::cout << "bytes read <= 0" << std::endl;
        closeConnection(i);
        return ;
    }
	else
	{
		std::cout << BG_GREEN << "Client " << i << " sent a request" << RESET << std::endl;
		HttpRequest parsedRequest(buffer);
		//std::cout << BG_BOLD_WHITE << buffer << RESET << std::endl;
		//parsedRequest.printRequest();
		//ListeningSocket* new_client = findSocket(i);
		//if (client)
			client->updateTime();
			//std::cout << BG_RED "socket null" RESET << std::endl;
		//(void)	client;
		_pendingResponses[i] = new TextResponse(parsedRequest);
		removeFromSet(i, _recv_fd_pool);
		addToSet(i, _write_fd_pool);
	}

}

void    ServerManager::sendResponse(const int &i, ListeningSocket* client)
{
	std::cout << BG_GREEN "sendResponse call fd: " RESET << i << std::endl;

	int bytes_sent;

	SimpleResponse* responsePtr = _pendingResponses[i];
    std::string response = responsePtr->getResponse();

	//std::cout << BG_BOLD_MAGENTA << response << RESET << std::endl;
	std::cout << "pointer is " << &responsePtr << std::endl;
	std::cout << BG_BOLD_YELLOW "size response" << response.size() << "fd is " << i  << RESET << std::endl;
    if (response.length() >= MESSAGE_BUFFER)
        bytes_sent = write(i, response.c_str(), MESSAGE_BUFFER);
    else
        bytes_sent = write(i, response.c_str(), response.length());

    if (bytes_sent < 0)
    {
        std::cout << BG_BOLD_YELLOW "Problem sending" RESET << std::endl;
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
		//ListeningSocket* client = findSocket(i);
		//(void)	client;
		client->updateTime();
		std::cout << BG_RED "time updated" << client->getLastTime() << RESET << std::endl;
		std::cout << BG_BOLD_RED "NOT entire data sent, bytes: " << bytes_sent << "  " << responsePtr->getResponse().size() << RESET << std::endl;
		responsePtr->cutRes(response, bytes_sent);
		std::cout << BG_BOLD_RED "NOT entire data sent, bytes: " << bytes_sent << "  " << responsePtr->getResponse().size() << RESET << std::endl;
	}
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