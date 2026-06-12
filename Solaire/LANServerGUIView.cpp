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
	m_gameName(gameName), m_advertiser(NULL)/*m_advertiser(new GameAdvertiser(gameName, playerName)), m_server(NULL)*/, deleteServerStuffOnHide(false), m_botLabel(NULL)
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

	//Bot controls (host only): add/remove AI bots that will spawn when the game starts.
	float botBtnW = 0.12f;
	float botBtnH = 0.06f;
	float botY = 1.0f - botBtnH - 0.02f;
	m_botLabel = env->addStaticText(L"Bots: 0", core::rect<s32>(static_cast<s32>(dim.Width*0.02f), static_cast<s32>(dim.Height*(botY-0.06f)), static_cast<s32>(dim.Width*0.3f), static_cast<s32>(dim.Height*(botY-0.01f))));
	m_botLabel->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_elements.push_back(m_botLabel);

	gui::IGUIButton* addBotBtn = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*0.02f), static_cast<s32>(dim.Height*botY), static_cast<s32>(dim.Width*(0.02f+botBtnW)), static_cast<s32>(dim.Height*(botY+botBtnH))), NULL, GUI_ID_LANFINAL_ADDBOT_BUTTON, L"Add Bot", L"Add an AI bot to the game");
	addBotBtn->setImage(driver->getTexture("TestButtonUp.tga"));
	addBotBtn->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	addBotBtn->setScaleImage(true);
	addBotBtn->setUseAlphaChannel(true);
	addBotBtn->setDrawBorder(false);
	m_elements.push_back(addBotBtn);

	gui::IGUIButton* removeBotBtn = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*(0.04f+botBtnW)), static_cast<s32>(dim.Height*botY), static_cast<s32>(dim.Width*(0.04f+2*botBtnW)), static_cast<s32>(dim.Height*(botY+botBtnH))), NULL, GUI_ID_LANFINAL_REMOVEBOT_BUTTON, L"Remove Bot", L"Remove an AI bot");
	removeBotBtn->setImage(driver->getTexture("TestButtonUp.tga"));
	removeBotBtn->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	removeBotBtn->setScaleImage(true);
	removeBotBtn->setUseAlphaChannel(true);
	removeBotBtn->setDrawBorder(false);
	m_elements.push_back(removeBotBtn);

	gui::IGUIButton* botDiffBtn = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*(0.06f+2*botBtnW)), static_cast<s32>(dim.Height*botY), static_cast<s32>(dim.Width*(0.06f+3*botBtnW)), static_cast<s32>(dim.Height*(botY+botBtnH))), NULL, GUI_ID_LANFINAL_BOTDIFF_BUTTON, L"Bot AI", L"Toggle bot difficulty (Normal / Hard)");
	botDiffBtn->setImage(driver->getTexture("TestButtonUp.tga"));
	botDiffBtn->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	botDiffBtn->setScaleImage(true);
	botDiffBtn->setUseAlphaChannel(true);
	botDiffBtn->setDrawBorder(false);
	m_elements.push_back(botDiffBtn);

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
	refreshBotLabel();
}

void LANServerGUIView::addBot()
{
	LANServer* server = NetworkController::get().getServer();
	if(server && server->getBotCount() < MAX_BOTS)
	{
		server->setBotCount(server->getBotCount() + 1);
		announceBotCount();
	}
	refreshBotLabel();
}

void LANServerGUIView::removeBot()
{
	LANServer* server = NetworkController::get().getServer();
	if(server && server->getBotCount() > 0)
	{
		server->setBotCount(server->getBotCount() - 1);
		announceBotCount();
	}
	refreshBotLabel();
}

void LANServerGUIView::toggleBotDifficulty()
{
	LANServer* server = NetworkController::get().getServer();
	if(server)
		server->setBotsAdvanced(!server->getBotsAdvanced());
	refreshBotLabel();
}

void LANServerGUIView::refreshBotLabel()
{
	if(!m_botLabel)
		return;
	LANServer* server = NetworkController::get().getServer();
	unsigned int n = server ? server->getBotCount() : 0;
	const bool hard = server ? server->getBotsAdvanced() : false;
	core::stringw t(L"Bots: ");
	t += (int)n;
	t += hard ? L"   (AI: Hard)" : L"   (AI: Normal)";
	m_botLabel->setText(t.c_str());
}

void LANServerGUIView::announceBotCount()
{
	LANServer* server = NetworkController::get().getServer();
	if(!server)
		return;
	//Let everyone in the lobby know the current bot count.
	core::stringw msg(L"* Bots in game: ");
	msg += (int)server->getBotCount();
	server->broadcastSystemMessage(msg.c_str());
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
