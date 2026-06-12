#define WIN64_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <winsock2.h>
#include <ws2tcpip.h>

#include "NetworkUtilities.h"


NetworkUtilities NetworkUtilities::m_NetworkUtilities;

NetworkUtilities& NetworkUtilities::get(){	
	return m_NetworkUtilities;
}

NetworkUtilities::NetworkUtilities(){
	m_errorMessages[WSAEINTR] = "Interrupted function call";
	m_errorMessages[WSAEBADF] = "WSAEBADF";
	m_errorMessages[WSAEACCES] = "WSAEACCES";
	m_errorMessages[WSAEFAULT] = "Bad address";
	m_errorMessages[WSAEINVAL] = "Invalid argument";
	m_errorMessages[WSAEMFILE] = "Too many open files";
	m_errorMessages[WSAEWOULDBLOCK] = "Operation would block";
	m_errorMessages[WSAEINPROGRESS] = "Operation now in progress";
	m_errorMessages[WSAEALREADY] = "Operation already in progress";
	m_errorMessages[WSAENOTSOCK] = "Socket operation on non-socket";
	m_errorMessages[WSAEDESTADDRREQ] = "Destination address required";
	m_errorMessages[WSAEMSGSIZE] = "Message too long";
	m_errorMessages[WSAEPROTOTYPE] = "Protocol wrong type for socket";
	m_errorMessages[WSAENOPROTOOPT] = "Bad protocol option";
	m_errorMessages[WSAEPROTONOSUPPORT] = "Protocol not supported";
	m_errorMessages[WSAESOCKTNOSUPPORT] = "Socket type not supported";
	m_errorMessages[WSAEOPNOTSUPP] = "Operation not supported";
	m_errorMessages[WSAEPFNOSUPPORT] = "Protocol family not supported";
	m_errorMessages[WSAEAFNOSUPPORT] = "Address family not supported by protocol family";
	m_errorMessages[WSAEADDRINUSE] = "Address already in use";
	m_errorMessages[WSAEADDRNOTAVAIL] = "Cannot assign requested address";
	m_errorMessages[WSAENETDOWN] = "Network is down";
	m_errorMessages[WSAENETUNREACH] = "Network is unreachable";
	m_errorMessages[WSAENETRESET] = "Network dropped connection on reset";
	m_errorMessages[WSAECONNABORTED] = "Software caused connection abort";
	m_errorMessages[WSAECONNRESET] = "Connection reset by peer";
	m_errorMessages[WSAENOBUFS] = "No buffer space available";
	m_errorMessages[WSAEISCONN] = "Socket is already connected";
	m_errorMessages[WSAENOTCONN] = "Socket is not connected";
	m_errorMessages[WSAESHUTDOWN] = "Cannot send after socket shutdown";
	m_errorMessages[WSAETOOMANYREFS] = "WSAETOOMANYREFS";
	m_errorMessages[WSAETIMEDOUT] = "Connection timed out";
	m_errorMessages[WSAECONNREFUSED] = "Connection refused";
	m_errorMessages[WSAELOOP] = "WSAELOOP";
	m_errorMessages[WSAENAMETOOLONG] = "WSAENAMETOOLONG";
	m_errorMessages[WSAEHOSTDOWN] = "Host is down";
	m_errorMessages[WSAEHOSTUNREACH] = "No route to host";
	m_errorMessages[WSAENOTEMPTY] = "WSAENOTEMPTY";
	m_errorMessages[WSAEPROCLIM] = "Too many processes";
	m_errorMessages[WSAEUSERS] = "WSAEUSERS";
	m_errorMessages[WSAEDQUOT] = "WSAEDQUOT";
	m_errorMessages[WSAESTALE] = "WSAESTALE";
	m_errorMessages[WSAEREMOTE] = "WSAEREMOTE";
	m_errorMessages[WSASYSNOTREADY] = "Network subsystem is unavailable";
	m_errorMessages[WSAVERNOTSUPPORTED] = "WINSOCK.DLL version out of range";
	m_errorMessages[WSANOTINITIALISED] = "Successful WSAStartup() not yet performed";
	m_errorMessages[WSAEDISCON] = "WSAEDISCON";
	m_errorMessages[WSAENOMORE] = "WSAENOMORE";
	m_errorMessages[WSAECANCELLED] = "WSAECANCELLED";
	m_errorMessages[WSAEINVALIDPROCTABLE] = "WSAEINVALIDPROCTABLE";
	m_errorMessages[WSAEINVALIDPROVIDER] = "WSAEINVALIDPROVIDER";
	m_errorMessages[WSAEPROVIDERFAILEDINIT] = "WSAEPROVIDERFAILEDINIT";
	m_errorMessages[WSASYSCALLFAILURE] = "WSASYSCALLFAILURE";
	m_errorMessages[WSASERVICE_NOT_FOUND] = "WSASERVICE_NOT_FOUND";
	m_errorMessages[WSATYPE_NOT_FOUND] = "WSATYPE_NOT_FOUND";
	m_errorMessages[WSA_E_NO_MORE] = "WSA_E_NO_MORE";
	m_errorMessages[WSA_E_CANCELLED] = "WSA_E_CANCELLED";
	m_errorMessages[WSAEREFUSED] = "WSAEREFUSED";
	m_errorMessages[WSAHOST_NOT_FOUND] = "Host not found";
	m_errorMessages[WSATRY_AGAIN] = "Non-authoritative host not found";
	m_errorMessages[WSANO_RECOVERY] = "This is a non-recoverable error";
	m_errorMessages[WSANO_DATA] = "Valid name, no data record of requested type";
}

NetworkUtilities::~NetworkUtilities(){

}

char* NetworkUtilities::getErrorMessage(const int error){
	std::map<int, char*>::const_iterator it = m_errorMessages.find(error);
	if(it != m_errorMessages.end()){
		return it->second;
	}
	return "";
}



char* getSocketError(const int error){
	return NetworkUtilities::get().getErrorMessage(error);
}

//this method returns the ip address of a client based on its information. it supports both ipv4 and ipv6 clients
void* getClientIP(sockaddr_storage* clientIP){
	void *ipAddress = NULL;
	if (clientIP->ss_family == AF_INET){
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)clientIP;
		ipAddress = &(ipv4->sin_addr);		
	}else if(clientIP->ss_family == AF_INET6){
		ipAddress = &( ((struct sockaddr_in6 *)clientIP)->sin6_addr );		
   	}
	/*
	char printableClientIP[INET6_ADDRSTRLEN];
	if(ipAddress){
		if(inet_ntop(clientIP->ss_family, ipAddress, printableClientIP, sizeof(printableClientIP)))
			return printableClientIP;
	}
	return "";
	*/
	return ipAddress;
}

//this method returns the port number of a client based on its information. it supports both ipv4 and ipv6 clients
u_short getClientPort(sockaddr_storage* clientPort){
	u_short portNumber;
	if (clientPort->ss_family == AF_INET){
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)clientPort;
		portNumber = ipv4->sin_port;
		return portNumber;
	}else{
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)clientPort;
		portNumber = ipv6->sin6_port;
		return portNumber;
   	}
}

/*
irr::core::stringw widen(char* ansistr)
{
	int lenA = lstrlenA(ansistr);
	int lenW;
	wchar_t* unicodestr;

	lenW = MultiByteToWideChar(CP_ACP, 0, ansistr, lenA, 0, 0);
	if (lenW > 0)
	{
		// Check whether conversion was successful
		unicodestr = SysAllocStringLen(ng0, lenW);
		if(MultiByteToWideChar(CP_ACP, 0, ansistr, lenA, unicodestr, lenW) != 0)
		{
			irr::core::stringw ret(unicodestr);
			SysFreeString(unicodestr);
			return ret;
		}
	}
	else
	{

		// handle the error
	}

	// when done, free the BSTR
	SysFreeString(unicodestr);

	return L"";
}

irr::core::stringc narrow(const irr::core::stringw& str)
{

	wchar_t* unicodestr = str.c_str();
	char *ansistr;
	int lenW = SysStringLen(unicodestr);
	int lenA = WideCharToMultiByte(CP_ACP, 0, unicodestr, lenW, 0, 0, NULL, NULL);
	if (lenA > 0)
	{
		ansistr = new char[lenA + 1]; // allocate a final null terminator as well
		if(WideCharToMultiByte(CP_ACP, 0, unicodestr, lenW, ansistr, lenA, NULL, NULL) != 0)
		{
			ansistr[lenA] = 0; // Set the null terminator yourself
			irr::core::stringc ret(ansistr);
			delete[] ansistr;
			SysFreeString(unicodestr);
			return ret;
		}
	}
	else
	{
		// handle the error
	}

	//...use the strings, then free their memory:
	delete[] ansistr;
	SysFreeString(unicodestr);

	return "";
}
*/