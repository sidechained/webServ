#include "Socket.hpp"

// Default constructor
Socket::Socket(int domain, int type, int protocol, int port, std::string ip, int backlog) : _ip(ip), _port(port), _buffer_read(false)
{
	(void)backlog;
	// Define address structure
	_address.sin_family = domain;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = inet_addr(ip.c_str());
	_addressSize = sizeof(_address);

	// Establish connection
	// print establish connection
	PRINT(SOCKET, CYAN, "\tFor server: " << ip << " on port: " << port << " Establishing connection...")
	_sock = socket(domain, type, protocol);
	testConnection(_sock);

	// Allow socket descriptor to be reuseable
	// Should allow to rerun ./server multiple times
	int reuse = 1;
	PRINT(SOCKET, CYAN, "\tFor server: " << ip << " on port: " << port << " Setting socket options...")
	testConnection(setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)));

	// Bind socket and IP
	PRINT(SOCKET, CYAN, "\tFor server: " << ip << " on port: " << port << " Binding socket...")
	_conection = bind(_sock, (struct sockaddr *)&_address, sizeof(_address));
	testConnection(_conection);

	// initialize last request time
	_last_request_time = time(NULL);

	PRINT(SOCKET, CYAN, "\tFor server: " << ip << " on port: " << port << " Listening on socket...")
	if (listen(_sock, backlog) == -1)
	{
		std::cerr << "listen error" << std::endl;
		exit(EXIT_FAILURE);
	}
	PRINT(SOCKET, CYAN, "\tFor server: " << ip << " on port: " << port << " Socket listening on fd: " << _sock)
	if (fcntl(_sock, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "fcntl error" << std::endl;
		exit(EXIT_FAILURE);
	}
	PRINT(SOCKET, CYAN, "\tFor server: " << ip << " on port: " << port << " Socket created with fd: " << _sock)
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

void Socket::testConnection(int item)
{
	if (item < 0)
	{
		perror("Failed to connect...");
		_exit(1);
	}
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

void Socket::appendBuffer(const char *buffer, int size)
{
	strcat(_buffer, buffer);
	for (int i = 0; i < size; i++)
	{
		_buffer_vector.push_back(buffer[i]);
	}
}

std::vector<char> Socket::getBufferVector() const
{
	return _buffer_vector;
}

char *Socket::getBuffer()
{
	return _buffer;
}

bool Socket::isBufferRead() const
{
	return _buffer_read;
}

void Socket::setBufferRead(bool read)
{
	_buffer_read = read;
}