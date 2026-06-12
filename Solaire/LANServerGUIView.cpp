#include <cassert>
#include <irrlicht.h>

#include "LANServerGUIView.h"
#include "NetworkController.h"
#include "GameAdvertiser.h"
#include "System.h"
#include "LANServer.h"
#include "TCPPacketFactory.h"
#include "TCPPacket.h"
#include "ChatHelper.h"
#include "GUIConstants.h"
#include "MenuScene.h"

using namespace irr;

LANServerGUIView::LANServerGUIView(MenuScene* parent, const wchar_t* playerName, const wchar_t* gameName) : LANFinalView(parent, playerName), 
	m_gameName(gameName), m_advertiser(NULL)/*m_advertiser(new GameAdvertiser(gameName, playerName)), m_server(NULL)*/, deleteServerStuffOnHide(false)
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
	core::dimension2d<u32> dim = driver->getScreenSize();

	float x = 0.3f;
	float y = 0.4f;
	float w = 0.1f;
	gui::IGUIStaticText* host = env->addStaticText(playerName, core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*y), static_cast<s32>(dim.Width*(x+w)), static_cast<s32>(dim.Height*(y+0.05f))));
	host->setOverrideColor(video::SColor(255, 255, 255, 255));	
	m_elements.push_back(host);

	
	float buttonWidth = 0.2f;
	float buttonHeight = buttonWidth*0.5f;

	gui::IGUIButton* startGame = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*(1.0f-buttonWidth)), static_cast<s32>(dim.Height*(1.0f-buttonHeight)), static_cast<s32>(dim.Width), static_cast<s32>(dim.Height)), NULL, GUI_ID_LANFINAL_STARTGAME_BUTTON, L"PLAY", L"Launches the game");
	startGame->setImage(driver->getTexture("TestButtonUp.tga"));
	startGame->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	startGame->setScaleImage(true);
	startGame->setUseAlphaChannel(true);
	startGame->setDrawBorder(false);
	m_elements.push_back(startGame);

	NetworkController::get().unregisterLANServer();
	/*
	m_server = new LANServer(playerName, m_advertiser, this); //avoiding to instantiate it in the initializer list because of the need for the "this" keyword
	NetworkController::get().registerLANServer(m_server);

	m_advertiser->setPaused(true);
	m_advertiser->start();
	m_server->setup();
	m_server->setPaused(true);
	m_server->start();
	*/
	hide();
}

LANServerGUIView::~LANServerGUIView()
{
	hide();
	
	if(m_advertiser)
	{
		//m_advertiser->finish();
		delete m_advertiser;
		m_advertiser = NULL;
	}
}

void LANServerGUIView::show()
{
	bool needToAdvertise = false;
	if(NULL == m_advertiser)
	{
		m_advertiser = new GameAdvertiser(m_gameName.c_str(), getPlayerName());
		needToAdvertise = true;
		//m_advertiser->start();
	}
	LANServer* server = NetworkController::get().getServer();
	if(NULL == server)
	{
		server = new LANServer(getPlayerName(), m_advertiser, this);
		//NetworkController::get().registerLANServer(m_server);
		bool serverInit = server->setup();
		if(!serverInit)
		{
			delete m_advertiser;
			m_advertiser = NULL;
			delete server;
			hide();
			m_parent->showLANView();
			return;
		}
		else
		{
			NetworkController::get().registerLANServer(server);
			if(needToAdvertise)
			{
				m_advertiser->start();
			}
			server->start();
		}
	}

	m_advertiser->setType(ADVERTISE);
	m_advertiser->setPaused(false);
	server->setPaused(false);

	GUIView::show();
}

void LANServerGUIView::hide()
{
	if(m_advertiser)
	{
		m_advertiser->setType(UNADVERTISE);
	}
	GUIView::hide();
	LANServer* server = NetworkController::get().getServer();
	if(server) 
		server->disconnectEveryone();
}

void LANServerGUIView::sendChatText(const wchar_t* playerName, const wchar_t* text)
{
	ChatHelper helper;
	helper.playername = playerName;
	helper.text = text;
	TCPPacket* packet = TCPPacketFactory::get().createPacket(SEND_CHAT_TEXT, &helper, 0);
	LANServer* server = NetworkController::get().getServer();
	if(packet)
	{
		if(server)
		{
			server->sendPacket(packet);
			LANFinalView::sendChatText(playerName, text);
		}

		delete packet;
	}
}

void LANServerGUIView::switchToLANScene()
{
	System::get().toggle(SCENE_LAN_SERVER);
}
