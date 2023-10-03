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
#include "../WebServ.hpp"
#include <cstring>
#include <vector>
//#include "../Servers/ServerManager.hpp"
//#define MAX_CLIENTS 10

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
	char _buffer[REQUEST_BUFFER];
	bool _buffer_read;
	std::vector<char> _buffer_vector;

public:
	Socket(int domain, int type, int protocol, int port, std::string ip, int backlog);
	~Socket();
	const time_t &getLastTime() const;
	void updateTime();

	void bindSocket();
	void testConnection(int item);
	sockaddr_in getAddress() const;
	int getSock() const;
	int getConnection() const;
	int getAddressSize() const;
	int getPort() const;
	std::string getIp() const;
	void appendBuffer(const char *buffer, int size);
	char* getBuffer();
	bool isBufferRead() const;
	void setBufferRead(bool read);
	std::vector<char> getBufferVector() const;
};
