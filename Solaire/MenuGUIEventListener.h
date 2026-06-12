#pragma once
#include "IEventListener.h"

class MenuScene;

class MenuGUIEventListener : public IEventListener
{
public:
	MenuGUIEventListener(MenuScene* scene);
	~MenuGUIEventListener();

	bool onGUIEvent(const irr::SEvent::SGUIEvent& evt);

private:

	void generateErrorMessageBoxForJoinFailure(const int errorCode);

	void createLANNewGame();
	void joinLANGame();
	void joinLANGameByIP();

	//Shows the "leave the LAN game?" confirmation and remembers which main-menu button the
	//user pressed, so we can navigate there once they confirm.
	bool promptLeaveLobbyIfNeeded(const irr::s32 destinationButtonId);
	void doLobbyExitNavigation();

	MenuScene* m_scene;
	irr::s32 m_pendingLobbyExit;//main-menu button id to act on after confirming a lobby leave
};

