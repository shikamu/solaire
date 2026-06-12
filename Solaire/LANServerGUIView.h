#pragma once
#include "LANFinalView.h"

class LANServer;
class GameAdvertiser;

class LANServerGUIView : public LANFinalView
{
public:
	LANServerGUIView(MenuScene* parent, const wchar_t* playerName, const wchar_t* gameName);
	~LANServerGUIView();

	void hide();
	void show();

	void sendChatText(const wchar_t* playerName, const wchar_t* text);

	void switchToLANScene();

private:

	irr::core::stringw m_gameName;
	
	GameAdvertiser* m_advertiser;
	//LANServer* m_server;
	bool deleteServerStuffOnHide;

};

