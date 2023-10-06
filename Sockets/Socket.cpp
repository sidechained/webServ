#include "Socket.hpp"

// Default constructor
Socket::Socket(int domain, int type, int protocol, int port, std::string ip, int backlog) : _domain(domain), _type(type), _protocol(protocol), _ip(ip), _port(port), _backlog(backlog)
{
	
}

bool Socket::createSocket() {
	// Define address structure
	_address.sin_family = _domain;
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = inet_addr(_ip.c_str());
	_addressSize = sizeof(_address);

	// Establish connection
	// print establish connection
	PRINT(SOCKET, CYAN, "\tFor server: " << _ip << " on port: " << _port << " Establishing connection...")
	_sock = socket(_domain, _type, _protocol);
	if (connectionFailed(_sock))
		return false;

	// Allow socket descriptor to be reuseable
	// Should allow to rerun ./server multiple times
	int reuse = 1;
	PRINT(SOCKET, CYAN, "\tFor server: " << _ip << " on port: " << _port << " Setting socket options...")
	if (connectionFailed(setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))))
		return false;

	// Bind socket and IP
	PRINT(SOCKET, CYAN, "\tFor server: " << _ip << " on port: " << _port << " Binding socket...")
	_conection = bind(_sock, (struct sockaddr *)&_address, sizeof(_address));
	if (connectionFailed(_conection))
		return false;

	// initialize last request time
	_last_request_time = time(NULL);

	PRINT(SOCKET, CYAN, "\tFor server: " << _ip << " on port: " << _port << " Listening on socket...")
	if (listen(_sock, _backlog) == -1)
	{
		std::cerr << "listen error" << std::endl;
		return false;
	}
	PRINT(SOCKET, CYAN, "\tFor server: " << _ip << " on port: " << _port << " Socket listening on fd: " << _sock)
	if (fcntl(_sock, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "fcntl error" << std::endl;
		return false;
	}
	PRINT(SOCKET, CYAN, "\tFor server: " << _ip << " on port: " << _port << " Socket created with fd: " << _sock)
	return true;
}

const time_t &Socket::getLastTime() const
{
	return _last_request_time;
}
void Socket::updateTime()
{
	_last_request_time = time(NULL);
	// std::cout << "Last request time set to " << _last_request_time << std::endl;
}

Socket::~Socket()
{
	close(_sock);
}

bool Socket::connectionFailed(int item)
{
	if (item < 0)
	{
		perror("Failed to connect...");
		return true;
	}
	return false;
}

// Getter functions

sockaddr_in Socket::getAddress() const
{
	return _address;
}

int Socket::getSock() const
{
	return _sock;
}

int Socket::getConnection() const
{
	return _conection;
}

int Socket::getAddressSize() const
{
	return _addressSize;
}

int Socket::getPort() const
{
	return _port;
}

std::string Socket::getIp() const
{
	return _ip;
}