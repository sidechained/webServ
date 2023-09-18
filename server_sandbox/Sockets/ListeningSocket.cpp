#include "ListeningSocket.hpp"

ListeningSocket::ListeningSocket(int domain, int type, int protocol, int port, std::string ip, int backlog) : SimpleSocket(domain, type, protocol, port, ip)
{
	
    (void) backlog;
    //startListening();
    //testConnection(_listening);
}

ListeningSocket::~ListeningSocket()
{
}

void ListeningSocket::startListening()
{
    //_listening = listen(getSock(), _backlog);
}