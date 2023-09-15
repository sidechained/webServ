#include "PollingServer.hpp"

PollingServer::PollingServer(int domain, int type, int protocol, int port, std::string ip, int backlog) : SimpleServer(domain, type, protocol, port, ip, backlog)
{
	_fds[0].fd = getSocket()->getSock();
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
		sockaddr_in address = getSocket()->getAddress();
		int addressSize = getSocket()->getAddressSize();
		int newSocket = accept(getSocket()->getSock(), (struct sockaddr *)&address, (socklen_t *)&addressSize);
		testConnection(newSocket);
		if (_clients < MAX_CLIENTS)
		{
			_clients++;
			_fds[_clients].fd = newSocket;
			_fds[_clients].events = POLLIN;
			_fds[_clients].revents = 0;
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
}

void PollingServer::responder()
{
	// Still needs to use poll to check if the socket is writable
	for (int i = 1; i <= _clients; i++)
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