#pragma once
#include "LogicScene.h"

class GUIView;
class SinglePlayerGUIView;
class OptionsGUIView;
class MenuGUIEventListener;
class LANGUIView;
class LANFinalView;

class MenuScene : public LogicScene
{
public:
	MenuScene();
	~MenuScene();

	int init();

	int update(const float dt);

	int clean();

	irr::scene::ISceneManager* getSceneManager() const;

	void showSinglePlayerView();
	void showLANView();
	void showLANServerView(const wchar_t* playername, const wchar_t* gamename);
	void showLANClientView(const wchar_t* playername);
	void showOptionsView();

	void toggleNextShip();
	void togglePreviousShip();

	void onShowControlsPanel();
	void onHideControlsPanel();

	void unlockKeybindingTable();
	bool isKeybindingTableLocked() const;

	void registerNewGame(wchar_t* gameName, wchar_t* ip, const unsigned int numberPlayers);
	void unregisterNewGame(wchar_t* gameName, wchar_t* ip);
	
	const wchar_t* getPlayerName() const;

	/*
	 * return codes for this functions are as follows:
	 * -1: unknown error
	 * 0 : successfully connected to the game
	 * 1 : problem with getaddrinfo(), ip address or port may be corrupted
	 * 2 : failed to create socket
	 * 3 : failed to connect to the server
	 * 4 : sending packet SEND_NAME failed
	 * 5 : created SEND_NAME packet failed
	 * 6 : current view in MenuScene is not m_lanView
	 * 7 : not receiving the ACCEPT_NAME packet
	 * 8 : couldn't configure socket (ioctlsocket failure)
	 */
	int connectToGame(const wchar_t* gameIP, const wchar_t* playerName);

private:
	
	irr::scene::ISceneManager* m_smgr;
	
	GUIView* m_current;

	GUIView* m_mainMenuView;
	SinglePlayerGUIView* m_singlePlayerView;
	LANGUIView* m_lanView;
	OptionsGUIView* m_optionsView;
	LANFinalView* m_lanFinalView;

	MenuGUIEventListener* m_listener;
};

