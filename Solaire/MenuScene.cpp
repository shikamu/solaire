#include <irrlicht.h>

#include "MenuScene.h"
#include "MainMenuGUIView.h"
#include "SinglePlayerGUIView.h"
#include "OptionsGUIView.h"
#include "LANGUIView.h"
#include "LANServerGUIView.h"
#include "LANClientGUIView.h"
#include "System.h"
#include "MenuGUIEventListener.h"
#include "GameAdvertiser.h"
#include "NetworkController.h"

using namespace irr;

MenuScene::MenuScene() : LogicScene(false), m_current(NULL), m_mainMenuView(NULL), m_singlePlayerView(NULL), m_lanView(NULL), m_optionsView(NULL), m_lanFinalView(NULL), m_smgr(NULL), m_listener(NULL)
{
	
}

MenuScene::~MenuScene()
{
	clean();
}

int MenuScene::init()
{
	clean();

	m_smgr = System::get().getDevice()->getSceneManager()->createNewSceneManager(false);


	gui::IGUIEnvironment* env = ::System::get().getDevice()->getGUIEnvironment();
	gui::IGUISkin* skin = env->getSkin();
	gui::IGUIFont* font = env->getFont("agency18.xml");
	if (font)
		skin->setFont(font, gui::EGDF_BUTTON);

	font = env->getFont("serif12.xml");
	if(font)
		skin->setFont(font);
	
	font = env->getFont("serif10.xml");
	if(font)
		skin->setFont(font, gui::EGDF_TOOLTIP);


	m_mainMenuView = new MainMenuGUIView(this);
	m_singlePlayerView = new SinglePlayerGUIView(this);
	m_optionsView = new OptionsGUIView(this);
	m_lanView = new LANGUIView(this);

	m_current = NULL;

	m_listener = new MenuGUIEventListener(this);
	System::get().registerEventListener(m_listener);



	m_mainMenuView->show();
	return 0;
}

int MenuScene::update(const float)
{
	return 0;
}

int MenuScene::clean()
{
	m_current = NULL;

	NetworkController::get().unregisterFinalLANView();

	delete m_mainMenuView;
	m_mainMenuView = NULL;
	delete m_singlePlayerView;
	m_singlePlayerView = NULL;
	delete m_lanView;
	m_lanView = NULL;
	delete m_optionsView;
	m_optionsView = NULL;

	m_lanFinalView = NULL;
	/*
	delete m_lanFinalView;
	m_lanFinalView = NULL;
	*/

	if(m_listener)
	{
		System::get().unregisterEventListener(m_listener);
		delete m_listener;
		m_listener = NULL;
	}

	if(m_smgr)
	{
		m_smgr->clear();
		m_smgr->drop();
		m_smgr = NULL;
	}
	return 0;
}

irr::scene::ISceneManager* MenuScene::getSceneManager() const
{
	return m_smgr;
}

void MenuScene::showSinglePlayerView()
{
	if(m_current)
		m_current->hide();
	m_current = m_singlePlayerView;
	if(m_current)
		m_current->show();	
}

void MenuScene::showLANView()
{
	if(m_current)
		m_current->hide();
	m_current = m_lanView;
	if(m_current)
		m_current->show();
}

void MenuScene::showLANServerView(const wchar_t* playername, const wchar_t* gamename)
{
	//m_lanFinalView is deleted through NetworkController::get().registerFinalLANView which is called further below
	/*
	if(m_lanFinalView)
		delete m_lanFinalView;
	*/
	m_lanView->stopListening();

	NetworkController::get().unregisterFinalLANView();
	m_lanFinalView = new LANServerGUIView(this, playername, gamename);

	NetworkController::get().registerFinalLANView(m_lanFinalView);

	if(m_current)
		m_current->hide();
	m_current = m_lanFinalView;
	if(m_current)
		m_current->show();
}

void MenuScene::showLANClientView(const wchar_t* playername)
{
	//m_lanFinalView is deleted through NetworkController::get().registerFinalLANView which is called further below
	/*
	if(m_lanFinalView)
		delete m_lanFinalView;
	*/
	m_lanFinalView = new LANClientGUIView(this, playername, m_lanView->getSocket());
	
	if(m_current)
		m_current->hide();
	m_current = m_lanFinalView;
	
	NetworkController::get().registerFinalLANView(m_lanFinalView);

	if(m_current)
		m_current->show();
}

bool MenuScene::isShowingLobby() const
{
	return m_lanFinalView != NULL && m_current == m_lanFinalView;
}

void MenuScene::leaveLobby()
{
	//Disconnect first so the server frees our slot (the client/server destructor closes the
	//socket; the server sees the EOF and removes us).
	if(NetworkController::get().getClient())
		NetworkController::get().unregisterLANClient();
	if(NetworkController::get().getServer())
		NetworkController::get().unregisterLANServer();

	//Tear down the lobby view (deleted via NetworkController). Clear our own pointers so we
	//don't leave m_current/m_lanFinalView dangling at the freed view.
	const bool wasCurrent = (m_current == m_lanFinalView);
	NetworkController::get().unregisterFinalLANView();
	m_lanFinalView = NULL;
	if(wasCurrent)
		m_current = NULL;
}

void MenuScene::showOptionsView()
{
	if(m_current)
		m_current->hide();
	m_current = m_optionsView;
	if(m_current)
		m_current->show();
}

void MenuScene::toggleNextShip()
{
	m_singlePlayerView->toggleNextShip();
}

void MenuScene::togglePreviousShip()
{
	m_singlePlayerView->togglePreviousShip();
}

void MenuScene::onShowControlsPanel()
{
	m_optionsView->registerInputListener();
}

void MenuScene::onHideControlsPanel()
{
	m_optionsView->unregisterInputListener();
}

void MenuScene::unlockKeybindingTable()
{
	m_optionsView->setKeybindingTableLocked(false);
}

bool MenuScene::isKeybindingTableLocked() const
{
	return m_optionsView->isKeybindingTableLocked();
}

void MenuScene::registerNewGame(wchar_t* gameName, wchar_t* ip, const unsigned int numberPlayers)
{
	if(m_current && m_current == m_lanView)
	{
		m_lanView->registerNewGame(gameName, ip, numberPlayers);
	}
}

void MenuScene::unregisterNewGame(wchar_t* gameName, wchar_t* ip)
{
	if(m_current && m_current == m_lanView)
	{
		m_lanView->unregisterNewGame(gameName, ip);
	}
}

const wchar_t* MenuScene::getPlayerName() const
{
	if(m_current == m_lanFinalView)
	{
		return m_lanFinalView->getPlayerName();
	}
	return L"";
}

int MenuScene::connectToGame(const wchar_t* gameIP, const wchar_t* playerName)
{
	if(m_current == m_lanView)
	{
		return m_lanView->connect(gameIP, playerName);
	}
	return 6;
}
