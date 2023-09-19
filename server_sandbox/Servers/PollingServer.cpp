#include "PollingServer.hpp"

PollingServer::PollingServer(ServerConfig &config) : SimpleServer(config)
{
	_fds[0].fd = getSockets()[0]->getSock();
	_fds[0].events = POLLIN;
	_clients = 0;
}

PollingServer::PollingServer(int domain, int type, int protocol, std::vector<int> ports, std::string ip, int backlog) : SimpleServer(domain, type, protocol, ports, ip, backlog)
{
	_fds[0].fd = getSockets()[0]->getSock();
	_fds[0].events = POLLIN;
	_clients = 0;
}

PollingServer::~PollingServer()
{
}

void PollingServer::accepter()
{
	if (_fds[0].revents & POLLIN)
	{
		sockaddr_in address = getSockets()[0]->getAddress();
		int addressSize = getSockets()[0]->getAddressSize();
		int newSocket = accept(getSockets()[0]->getSock(), (struct sockaddr *)&address, (socklen_t *)&addressSize);
		testConnection(newSocket);
		if (_clients < MAX_CLIENTS)
		{
			_clients++;
			_fds[_clients].fd = newSocket;
			_fds[_clients].events = POLLIN | POLLOUT;
			_fds[_clients].revents = 0;
			std::cout << BG_GREEN << "New client connected" << RESET << std::endl;
			printClientAddress(address);
		}
		else
		{
			log("Too many clients. Connection rejected.");
			close(newSocket);
		}
	}
}

void PollingServer::handler()
{
	for (int i = 1; i <= _clients; i++)
	{
		if (_fds[i].revents & POLLIN)
		{
			memset(_buffer, 0, sizeof(_buffer));
			int bytesRead = read(_fds[i].fd, _buffer, sizeof(_buffer));
			std::cout << "Reading from client " << i << std::endl;
			if (bytesRead > 0)
			{
				std::cout << BG_GREEN << "Client " << i << " sent a request" << RESET << std::endl;
				std::cout << GREEN << "Request: " << _buffer << RESET << std::endl;
				HttpRequest parsedRequest(_buffer, _config);
				parsedRequest.printRequest();
				_pendingResponses[_fds[i].fd] = TextResponse(parsedRequest);
				_pendingResponses[_fds[i].fd].printResponse();
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
}

void PollingServer::responder()
{
	for (int i = 1; i <= _clients; i++)
	{
		if (_fds[i].revents & POLLOUT)
		{
			int clientFd = _fds[i].fd;

			if (_pendingResponses.find(clientFd) == _pendingResponses.end())
				continue;

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
				{
					response.setHeaderSent(true);
					std::cout << BG_BOLD_GREEN << "Header sent to client " << i << RESET << std::endl;
					std::cout << BG_BOLD_MAGENTA << "Header: " << RESET << std::endl;
					std::cout << header << std::endl;
				}
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
				{
					response.setBodySent(true);
					std::cout << BG_BOLD_GREEN << "Body sent to client " << i << RESET << std::endl;
				}
				else
					response.updateBodyOffset(bytes_sent);
			}
			// Remove the response from the map if it has been sent
			if (response.isHeaderSent() && response.isBodySent())
			{
				_pendingResponses.erase(clientFd);
			}
		}
	}
}

void PollingServer::launch()
{
	while (isRunning)
	{
		_poll = poll(_fds, _clients + 1, -1);
		if (_poll == -1)
			break;
		accepter();
		handler();
		responder();
	}
}