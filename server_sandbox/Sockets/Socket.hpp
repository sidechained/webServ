#pragma once
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <cstdlib>
#include "../Colors.hpp"
#define MAX_CLIENTS 10

class Socket
{
protected:
	sockaddr_in _address;
	int _sock;
	int _conection;
	int _addressSize;
	time_t _last_request_time;
	std::string _ip;
	int _port;

public:
	Socket(int domain, int type, int protocol, int port, std::string ip, int backlog);
	~Socket();
	Socket(const Socket &other);
	const time_t &getLastTime() const;
	void updateTime();
	// Socket &operator=(const Socket &other);

	void bindSocket();
	void testConnection(int item);
	sockaddr_in getAddress() const;
	int getSock() const;
	int getConnection() const;
	int getAddressSize() const;
	int getPort() const;
	std::string getIp() const;
};
