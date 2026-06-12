#pragma once
#include <winsock2.h>
#include "LANFinalView.h"

namespace irr
{
	namespace gui
	{
		class IGUIButton;
	}
}

class LANClient;

class LANClientGUIView : public LANFinalView
{
public:
	LANClientGUIView(MenuScene* parent, const wchar_t* name, SOCKET socket);
	~LANClientGUIView();

	void show();
	void hide();

	void sendChatText(const wchar_t* playerName, const wchar_t* text);

	void switchToLANScene();

	//Flips this client's ready state, updates the button label and tells the server.
	void toggleReady();

private:

	LANClient* m_client;

	SOCKET m_socket;

	bool m_ready;
	irr::gui::IGUIButton* m_readyButton;
};

