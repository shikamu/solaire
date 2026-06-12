#pragma once
#include <winsock2.h>

#include <irrString.h>
#include "Task.h"
#include "CSLock.h"


struct sockaddr_storage;

enum AdvertType
{
	ADVERTISE,
	UNADVERTISE

};

class GameAdvertiser : public Task
{
public:
	GameAdvertiser(const wchar_t* gameName, const wchar_t* playerName);
	~GameAdvertiser();

	void setPaused(const bool b);

	void setType(const AdvertType type);

	void setNumberPlayers(const unsigned int n);

protected:
	unsigned int __stdcall run();

private:
	bool isPaused() const;

	bool init();

	void doJob();

	void cleanup();

	irr::core::stringw m_gameName, m_playerName;

	unsigned int m_numberPlayers;

	CSLock m_lock;

	//UDP stuff
	sockaddr_storage* m_broadcastAddressStorage;
	SOCKET m_broadcastSocket;
	int m_broadcastAddressSize;
	sockaddr* m_broadcastAddress;

	AdvertType m_type;

	bool m_paused;

	
	GameAdvertiser(const GameAdvertiser& other); 
	GameAdvertiser& operator=(const GameAdvertiser& other); 

};

