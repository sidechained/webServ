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
    ports.push_back(8080);
	//SimpleServer* server1 = new SimpleServer(AF_INET, SOCK_STREAM, 0, ports, "127.0.0.2", MAX_CLIENTS);
    //_servers.push_back(server1);
    _servers.push_back(new SimpleServer(AF_INET, SOCK_STREAM, 0, ports, "127.0.0.1", MAX_CLIENTS));
    //_servers.push_back(new SimpleServer(AF_INET, SOCK_STREAM, 0, ports, "127.0.0.3", MAX_CLIENTS));

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
            else if (FD_ISSET(i, &recv_set_cpy) && _clients_map.count(i)) {}
                //readRequest(i, _clients_map[i]);
            else if (FD_ISSET(i, &write_set_cpy) && _clients_map.count(i))
            {
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
	// ADDING THE SOCKETS TO THE coresponding MAP
    /*new_client.setSocket(client_sock);
    if (_clients_map.count(client_sock) != 0)
        _clients_map.erase(client_sock);
    _clients_map.insert(std::make_pair(client_sock, new_client));*/
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