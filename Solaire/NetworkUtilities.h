#pragma once
#define WIN64_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <map>
#include <WinSock2.h>

struct sockaddr_storage;

char* getSocketError(const int error);

//this method returns the ip address of a client based on its information. it supports both ipv4 and ipv6 clients
void* getClientIP(sockaddr_storage* clientIP);

//this method returns the port number of a client based on its information. it supports both ipv4 and ipv6 clients
u_short getClientPort(sockaddr_storage* clientPort);


class NetworkUtilities
{
public:

	static NetworkUtilities& get();

	char* getErrorMessage(const int error);

private:
	NetworkUtilities();
	~NetworkUtilities();

	static NetworkUtilities m_NetworkUtilities;
	std::map<int, char*> m_errorMessages;
};


