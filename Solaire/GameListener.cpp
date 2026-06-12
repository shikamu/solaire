#include <ws2tcpip.h>
#include <irrlicht.h>

#include "GameListener.h"

#include "System.h"
#include "NetworkUtilities.h"
#include "MenuScene.h"
#include "NetworkConstants.h"

using namespace irr;


GameListener::GameListener(MenuScene* scene): m_scene(scene),
	m_broadcastAddressStorage(NULL), m_broadcastSocket(INVALID_SOCKET), m_broadcastAddressSize(0), m_broadcastAddress(NULL), m_paused(false)
{
}

GameListener::~GameListener()
{
	cleanup();
	finish();
}

void GameListener::setPaused(const bool b)
{
	m_paused = b;
}

unsigned int __stdcall GameListener::run()
{	
	if(init())
	{
		while(!isInterrupted())
		{
			Sleep(25);
			if(!isPaused())
			{
				doJob();
			}
		}
	}
	else
	{
		System::get().log("failed to initialize game listener");
	}
	return 0;
}

bool GameListener::isPaused() const
{
	return m_paused;
}

bool GameListener::init()
{
	System::get().log("Preparing to look for games...");

	if(m_broadcastAddressStorage)
	{
		delete m_broadcastAddressStorage;
		m_broadcastAddressStorage = NULL;
	}

	u_short port = 7777;
	m_broadcastAddressStorage = new sockaddr_storage;
	memset(m_broadcastAddressStorage, 0, sizeof(sockaddr_storage));
	sockaddr_in* destAddr4 = (sockaddr_in*)m_broadcastAddressStorage;
	destAddr4->sin_family = AF_INET;
	destAddr4->sin_port = htons(port);
	destAddr4->sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	m_broadcastAddressSize = sizeof(sockaddr_in);

	m_broadcastAddress = (sockaddr*)m_broadcastAddressStorage;

	m_broadcastSocket = socket(m_broadcastAddress->sa_family, SOCK_DGRAM, IPPROTO_UDP);
	if(m_broadcastSocket == INVALID_SOCKET){
		core::stringc errorMsg("Create socket failed with ");
		errorMsg+=getSocketError(WSAGetLastError());
		System::get().log(errorMsg.c_str());
		return false;
	}

	//Allow several sockets to bind this same broadcast port. Without this, only the FIRST
	//process on the machine can bind UDP 7777; any other instance's bind() fails and that
	//instance never receives game advertisements (its server list stays empty). On Windows
	//SO_REUSEADDR also makes incoming broadcasts get delivered to ALL bound sockets, which is
	//exactly what we want so every instance on the box can discover games.
	char reuse = 1;
	if(setsockopt(m_broadcastSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == SOCKET_ERROR){
		core::stringc errorMsg("setsockopt(SO_REUSEADDR) failed with ");
		errorMsg+=getSocketError(WSAGetLastError());
		System::get().log(errorMsg.c_str());
		return false;
	}

	if(bind(m_broadcastSocket, m_broadcastAddress, m_broadcastAddressSize) == SOCKET_ERROR){
		core::stringc errorMsg("Bind failed with ");
		errorMsg+=getSocketError(WSAGetLastError());
		System::get().log(errorMsg.c_str());
		return false;
	}
	//going to use non-blocking socket so that it's easier to stop advertising the game (e.g. if the host decided not to create it after all, by pressing 'back' or whatever)
	//set to non-blocking.
	u_long iMode = 1;
	int error = ioctlsocket(m_broadcastSocket, FIONBIO, &iMode);
	if(error != 0){
		core::stringc errorMsg("ioctlsocket failed with ");
		errorMsg+=getSocketError(WSAGetLastError());
		System::get().log(errorMsg.c_str());
		return false;
	}

	return true;}

void GameListener::doJob()
{
	sockaddr_storage clientAddr;
	memset(&clientAddr, 0, sizeof(sockaddr_storage));

	int addrSize = sizeof(sockaddr_in);

	char* test = new char[4+1+1+1+20*WCHAR_SIZE+14*WCHAR_SIZE];
	int p = 0;
	if((p = recvfrom(m_broadcastSocket, test, 4+1+1+1+20*WCHAR_SIZE+14*WCHAR_SIZE, 0, (sockaddr*)&clientAddr, &addrSize)) == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if(error != WSAEWOULDBLOCK)
		{
			irr::core::stringc errorMsg("recvfrom failed with ");
			errorMsg += getSocketError(WSAGetLastError());
			System::get().log(errorMsg.c_str());
		}
	}
	else
	{
		unsigned int messageSize = 0;
		unsigned int type = 0;
		unsigned int gameNameSize = 0;
		unsigned int playerNameSize = 0;
		unsigned int numberOfPlayers = 0;

		memcpy(&messageSize, test, 4);
		memcpy(&type, test+4, 1);

		memcpy(&gameNameSize, test+4+1, 1);
		memcpy(&playerNameSize, test+4+1+1, 1);
		memcpy(&numberOfPlayers, test+4+1+1+1, 1);
		

		wchar_t* gameName = new wchar_t[gameNameSize];
		mbstowcs_s(&gameNameSize, gameName, gameNameSize, test+4+1+1+1+1, gameNameSize);

		wchar_t* playerName = new wchar_t[playerNameSize];
		mbstowcs_s(&playerNameSize, playerName, playerNameSize, test+4+1+1+1+1+gameNameSize, playerNameSize);

		void* serverIP = getClientIP(&clientAddr);
		char printableClientIP[INET6_ADDRSTRLEN];
		inet_ntop(clientAddr.ss_family, serverIP, printableClientIP, sizeof(printableClientIP));		

		size_t serverIPSize;
		mbstowcs_s(&serverIPSize, NULL, 0, printableClientIP, _TRUNCATE);
		wchar_t* ip = new wchar_t[serverIPSize];
		mbstowcs_s(&serverIPSize, ip, serverIPSize, printableClientIP, _TRUNCATE);
		
		
		if(type == 0)
		{
			System::get().log("Received register game");
			m_scene->registerNewGame(gameName, ip, numberOfPlayers);
		}
		else
		{
			System::get().log("Received unregister game");
			m_scene->unregisterNewGame(gameName, ip);
		}


		delete[] ip;
		delete[] gameName;
		delete[] playerName;
	}
	delete[] test;
}

void GameListener::cleanup()
{
	m_paused = true;
	interrupt();
	shutdown(m_broadcastSocket, SD_BOTH);
	closesocket(m_broadcastSocket);
	if(m_broadcastAddressStorage)
	{
		delete m_broadcastAddressStorage;
		m_broadcastAddressStorage = NULL;
	}
}

