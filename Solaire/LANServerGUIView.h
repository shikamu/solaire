#pragma once
#include "LANFinalView.h"

namespace irr
{
	namespace gui
	{
		class IGUIStaticText;
	}
}

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

	void addBot();
	void removeBot();
	void toggleBotDifficulty();

	void switchToLANScene();

private:

	void refreshBotLabel();
	void announceBotCount();

	irr::core::stringw m_gameName;

	GameAdvertiser* m_advertiser;
	//LANServer* m_server;
	bool deleteServerStuffOnHide;

	irr::gui::IGUIStaticText* m_botLabel;

};

