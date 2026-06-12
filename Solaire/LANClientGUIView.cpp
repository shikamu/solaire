#include <irrlicht.h>

#include "LANClientGUIView.h"
#include "LANClient.h"
#include "TCPPacketFactory.h"
#include "TCPPacket.h"
#include "ChatHelper.h"
#include "System.h"
#include "GUIConstants.h"
#include "NetworkController.h"

using namespace irr;

LANClientGUIView::LANClientGUIView(MenuScene* parent, const wchar_t* name, SOCKET socket) : LANFinalView(parent, name), m_client(new LANClient(socket, name)), m_socket(socket)
{
	NetworkController::get().registerLANClient(m_client);
	
	m_client->setPaused(true);
	m_client->start();

	hide();
}

LANClientGUIView::~LANClientGUIView()
{
	
	hide();
	/*
	if(m_client)
	{
		delete m_client;
		m_client = NULL;
	}
	*/
}

void LANClientGUIView::show()
{
	m_client->setPaused(false);

	GUIView::show();
}

void LANClientGUIView::hide()
{
	
	GUIView::hide();
}

void LANClientGUIView::sendChatText(const wchar_t* playerName, const wchar_t* text)
{
	ChatHelper helper;
	helper.playername = playerName;
	helper.text = text;
	TCPPacket* packet = TCPPacketFactory::get().createPacket(SEND_CHAT_TEXT, &helper, 0);
	if(packet)
	{
		if(packet->send(m_socket))
		{
			System::get().log("client successfully sent chat text");
		}
		else
		{
			System::get().log("client failed to sent chat text");
		}
		delete packet;
	}
}

void LANClientGUIView::switchToLANScene()
{
	System::get().toggle(SCENE_LAN_CLIENT);
}
