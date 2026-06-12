#pragma once
#include <winsock2.h>
#include "LANFinalView.h"

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

private:
	
	LANClient* m_client;

	SOCKET m_socket;
};

