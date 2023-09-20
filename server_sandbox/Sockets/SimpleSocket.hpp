#pragma once
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class SimpleSocket
{
protected:
    sockaddr_in _address;
    int _sock;
    int _conection;
    int _addressSize;
	time_t _last_request_time;
public:
    SimpleSocket(int domain, int type, int protocol, int port, std::string ip);
    ~SimpleSocket();
	SimpleSocket(const SimpleSocket &other);
	const time_t &getLastTime() const;
	void updateTime();
	//SimpleSocket &operator=(const SimpleSocket &other);

    void bindSocket();
    void testConnection(int item);
    sockaddr_in getAddress() const;
    int getSock() const;
    int getConnection() const;
    int getAddressSize() const;
};
