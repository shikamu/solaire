#pragma once
#include <winsock2.h>

#include "Task.h"

class MenuScene;

class GameListener : public Task
{
public:
	GameListener(MenuScene* scene);
	~GameListener();

	void setPaused(const bool b);

protected:
	unsigned int __stdcall run();

private:

	bool isPaused() const;

	bool init();

	void doJob();

	void cleanup();

	MenuScene* m_scene;

	//UDP stuff
	sockaddr_storage* m_broadcastAddressStorage;
	SOCKET m_broadcastSocket;
	int m_broadcastAddressSize;
	sockaddr* m_broadcastAddress;
	bool m_paused;

};

