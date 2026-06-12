#pragma once
#include <winsock2.h>
#include <irrString.h>
#include "Task.h"

class TCPPacket;

class LANClient : public Task
{

public:
	LANClient(SOCKET socket, const wchar_t* playerName);
	~LANClient();

	void setPaused(const bool b);

	//this function does not delete the packet
	void sendPacket(TCPPacket* packet);
	
	//void finish();

	const wchar_t* getName() const;

	void backToLANView();


protected:
	unsigned int __stdcall run();

private:

	void receiveData();

	irr::core::stringw m_playername;

	SOCKET m_socket;
	bool m_paused, m_showServerDisconnected;

};

