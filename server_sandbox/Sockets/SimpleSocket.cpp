#include "SimpleSocket.hpp"

// Default constructor
SimpleSocket::SimpleSocket(int domain, int type, int protocol, int port, std::string ip)
{
    // Define address structure
    _address.sin_family = domain;
    _address.sin_port = htons(port);
    _address.sin_addr.s_addr = inet_addr(ip.c_str());
    _addressSize = sizeof(_address);

    // Establish connection
    _sock = socket(domain, type, protocol);
    testConnection(_sock);

    // Allow socket descriptor to be reuseable
    // Should allow to rerun ./server multiple times
    int reuse = 1;
    testConnection(setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)));

    // Set socket to be nonblocking?

    // Bind socket and IP
    _conection = bind(_sock, (struct sockaddr *)&_address, sizeof(_address));
    testConnection(_conection);
}

SimpleSocket::~SimpleSocket()
{
    close(_sock);
}

void SimpleSocket::testConnection(int item)
{
    if (item < 0)
    {
        perror("Failed to connect...");
        _exit(1);
    }
}

// Getter functions
sockaddr_in SimpleSocket::getAddress() const
{
    return _address;
}

int SimpleSocket::getSock() const
{
    return _sock;
}

int SimpleSocket::getConnection() const
{
    return _conection;
}

int SimpleSocket::getAddressSize() const
{
    return _addressSize;
}