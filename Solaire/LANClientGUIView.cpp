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

LANClientGUIView::LANClientGUIView(MenuScene* parent, const wchar_t* name, SOCKET socket) : LANFinalView(parent, name), m_client(new LANClient(socket, name)), m_socket(socket),
	m_ready(false), m_readyButton(NULL)
{
	NetworkController::get().registerLANClient(m_client);

	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
	core::dimension2d<u32> dim = driver->getScreenSize();

	//Ready toggle in the bottom-right (where the host has PLAY). Clients ready up here and
	//the host can only start once everyone has.
	float buttonWidth = 0.2f;
	float buttonHeight = buttonWidth*0.5f;
	m_readyButton = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*(1.0f-buttonWidth)), static_cast<s32>(dim.Height*(1.0f-buttonHeight)), static_cast<s32>(dim.Width), static_cast<s32>(dim.Height)), NULL, GUI_ID_LANFINAL_READY_BUTTON, L"Ready", L"Toggle your ready status");
	m_readyButton->setImage(driver->getTexture("TestButtonUp.tga"));
	m_readyButton->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	m_readyButton->setScaleImage(true);
	m_readyButton->setUseAlphaChannel(true);
	m_readyButton->setDrawBorder(false);
	m_elements.push_back(m_readyButton);

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

void LANClientGUIView::toggleReady()
{
	m_ready = !m_ready;

	bool ready = m_ready;
	TCPPacket* packet = TCPPacketFactory::get().createPacket(PLAYER_READY, &ready, 0);
	if(packet)
	{
		packet->send(m_socket);
		delete packet;
	}

	if(m_readyButton)
		m_readyButton->setText(m_ready ? L"Not Ready" : L"Ready");
}

void LANClientGUIView::switchToLANScene()
{
	System::get().toggle(SCENE_LAN_CLIENT);
}
