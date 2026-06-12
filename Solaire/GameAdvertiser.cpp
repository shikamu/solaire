#include <irrlicht.h>

#include "GameAdvertiser.h"

#include "System.h"
#include "NetworkUtilities.h"

using namespace irr;

GameAdvertiser::GameAdvertiser(const wchar_t* gameName, const wchar_t* playerName) : m_gameName(gameName), m_playerName(playerName), 
	m_numberPlayers(1), m_broadcastAddressStorage(NULL), m_broadcastSocket(INVALID_SOCKET), m_broadcastAddressSize(0), m_broadcastAddress(NULL), m_type(ADVERTISE), m_paused(false)
{
}

GameAdvertiser::~GameAdvertiser()
{
	cleanup();
	finish();
}

void GameAdvertiser::setPaused(const bool b)
{
	m_paused = b;
}

void GameAdvertiser::setType(const AdvertType type)
{
	m_type = type;
}

void GameAdvertiser::setNumberPlayers(const unsigned int n)
{
	m_numberPlayers = n;
}

unsigned int __stdcall GameAdvertiser::run()
{
	if(init())
	{
		while(!isInterrupted())
		{
			//Sleep(25);
			m_lock.Lock();
			if(!isPaused())
			{
				doJob();
			}
			m_lock.Unlock();
		}
	}
	else
	{
		System::get().log("failed to initialize game advertiser");
	}
	return 0;
}

bool GameAdvertiser::isPaused() const
{
	return m_paused;
}

bool GameAdvertiser::init()
{
	System::get().log("Preparing to advertise...");

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
	destAddr4->sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);			
	m_broadcastAddressSize = sizeof(sockaddr_in);
	m_broadcastAddress = (sockaddr*)m_broadcastAddressStorage;

	m_broadcastSocket = socket(m_broadcastAddress->sa_family, SOCK_DGRAM, IPPROTO_UDP);
	if(m_broadcastSocket == INVALID_SOCKET)
	{
		core::stringc errorMsg("Create socket failed with ");
		errorMsg+=getSocketError(WSAGetLastError());
		System::get().log(errorMsg.c_str());
		return false;
	}

	//set socket to allow broadcast
	char broadcastPerm = 1;
	if(setsockopt(m_broadcastSocket, SOL_SOCKET, SO_BROADCAST, &broadcastPerm, sizeof(broadcastPerm)) == SOCKET_ERROR)
	{
		core::stringc errorMsg("setsockopt failed with ");
		errorMsg+=getSocketError(WSAGetLastError());
		System::get().log(errorMsg.c_str());
		return false;
	}

	//going to use non-blocking socket so that it's easier to stop advertising the game (e.g. if the host decided not to create it after all, by pressing 'back' or whatever)
	//set to non-blocking.
	u_long iMode = 1;
	int error = ioctlsocket(m_broadcastSocket, FIONBIO, &iMode);
	if(error != 0)
	{
		core::stringc errorMsg("ioctlsocket failed with ");
		errorMsg+=getSocketError(WSAGetLastError());
		System::get().log(errorMsg.c_str());
		return false;
	}
	return true;
}

void GameAdvertiser::doJob()
{
	//the message is:
	//4 bytes of length [header]
	//1 byte for the type; currently 0 means advertise the game (as in register it), 1 means unregister the game
	//1 byte of game name length ->GN
	//1 byte of player name length ->PN
	//1 byte number of players in the game
	//<GN> wchar_t
	//<PN> wchar_t
	

	size_t gameNameSize, playerNameSize;
	wcstombs_s(&gameNameSize, NULL, 0, m_gameName.c_str(), _TRUNCATE);
	char* gameName = new char[gameNameSize];
    wcstombs_s(&gameNameSize, gameName, gameNameSize, m_gameName.c_str(), _TRUNCATE);

	wcstombs_s(&playerNameSize, NULL, 0, m_playerName.c_str(), _TRUNCATE);
	char* playerName = new char[playerNameSize];
    wcstombs_s(&playerNameSize, playerName, playerNameSize, m_playerName.c_str(), _TRUNCATE);


	const unsigned int messageLength = 4 + 1 + 1 + 1 + 1 + gameNameSize + playerNameSize;	

	char* message = new char[messageLength];

	memcpy(message, (char*)&messageLength, 4);
	memcpy(message+4, (char*)&m_type, 1);
	memcpy(message+4+1, (char*)&gameNameSize, 1);
	memcpy(message+4+1+1, (char*)&playerNameSize, 1);
	memcpy(message+4+1+1+1, (char*)&m_numberPlayers, 1);

	memcpy(message+4+1+1+1+1, gameName, gameNameSize);
	memcpy(message+4+1+1+1+1+gameNameSize, playerName, playerNameSize);
	delete[] gameName;
	delete[] playerName;

	int numBytes = sendto(m_broadcastSocket, message, messageLength, 0, m_broadcastAddress, m_broadcastAddressSize);
	if(numBytes < 0)
	{
		irr::core::stringc errorMsg("Send failed with ");
		errorMsg += getSocketError(WSAGetLastError());
		System::get().log(errorMsg.c_str());
	}
	else if(numBytes != messageLength)
	{
		System::get().log("sendto() sent unexpected number of bytes");
	}
	else
	{
		System::get().log("successfully sent!");
	}
	delete[] message;
	Sleep(1000);
}

void GameAdvertiser::cleanup()
{
	interrupt();
	m_lock.Lock();
	shutdown(m_broadcastSocket, SD_BOTH);
	closesocket(m_broadcastSocket);
	if(m_broadcastAddressStorage)
	{
		delete m_broadcastAddressStorage;
		m_broadcastAddressStorage = NULL;
	}
	m_broadcastSocket = INVALID_SOCKET;
	m_lock.Unlock();
}
