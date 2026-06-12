#include <new>
#include <ctime>
#include <fstream>
#include <irrlicht.h>

#include "System.h"
#include "ConfigData.h"
#include "LogicScene.h"
#include "MenuScene.h"
#include "SinglePlayerScene.h"
#include "LANClientScene.h"
#include "LANServerScene.h"
#include "IEventListener.h"
#include "RenderManager.h"
#include "BasicLogEventListener.h"
#include "Timer.h"
#include "NetworkController.h"
#include "SpaceObjectNetworkInfo.h"
#include "SpaceObject.h"
#include "RenderObjectIncludes.h"
#include "LockPointer.h"
#include "PhysicsObject.h"
#include "LANGameCreationHelper.h"
#include "LogicConstants.h"
#include "GarbageCollector.h"
#include "AudioManager.h"
#include "GUIConstants.h"
#include "Agent.h"

#ifdef _IRR_WINDOWS_
//link library
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(lib, "Ws2_32.lib")
//get rid of the console window
//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#pragma comment(linker, "/subsystem:console /ENTRY:mainCRTStartup")
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#include <crtdbg.h>
int flaggggg = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF;
int ddjgsakagguzga = _CrtSetDbgFlag(flaggggg); //junk variable to allow making this call
#endif

System System::m_System;

using namespace irr;

static const bool DO_LOGGING = false;

::System::System() : m_config(new ConfigData), m_currentScene(NULL), m_menuScene(new MenuScene), m_singlePlayerScene(new SinglePlayerScene), m_lanClientScene(new LANClientScene), m_lanServerScene(new LANServerScene),
		m_running(true), m_terminated(false), m_pendingInit(false)
{
	/*
	m_config->createDevice();
	
	m_menuScene->init();
	m_singlePlayerScene->init();
	m_currentScene = m_menuScene;
	*/
	reinit();
}

System::~System()
{
	/*
	LANClient* client = NetworkController::get().getClient();
	if(client)
	{
		//client->interrupt();
		client->finish();
	}
	*/
	NetworkController::get().unregisterLANClient();
	/*
	LANServer* server = NetworkController::get().getServer();
	if(server)
	{
		//server->interrupt();
		server->finish();
	}
	*/

	NetworkController::get().unregisterFinalLANView();



	NetworkController::get().unregisterLANServer();

	m_currentScene = NULL;

	delete m_menuScene;
	m_menuScene = NULL;	

	delete m_singlePlayerScene;
	m_singlePlayerScene = NULL;

	delete m_lanClientScene;
	m_lanClientScene = NULL;
	
	delete m_lanServerScene;
	m_lanServerScene = NULL;


	//must delete config last because scenes may be unregistering eventlisteners from config when they get deleted, thus config must still exist
	delete m_config;
	m_config = NULL;
}

void System::reinit()
{
	m_sceneLock.Lock();
	m_config->createDevice();
	m_config->getKeybindings().readBindings();

	m_menuScene->setInitialized(false);

	m_currentScene = m_menuScene;
	m_pendingInit = true;
	m_sceneLock.Unlock();

	//m_menuScene->init();
}

System& System::get()
{
	return m_System;
}

IrrlichtDevice* System::getDevice() const
{
	return m_config->getDevice();
}

int System::run()
{
#ifdef _DEBUG
	BasicLogEventListener logListener;
	m_config->getEventReceiver().registerEventListener(&logListener);
#endif

	bool isFirstIteration = true;
	GarbageCollector::Get().start(); 
	FMODAudio::AudioManager::get().start();




	IrrlichtDevice* device = m_config->getDevice();
	while(isRunning())
	{
		if(!isFirstIteration)
		{
			reinit();
			device = m_config->getDevice();
			
			//this is because in theory we can only get here from the options menu. this might need changing later on though
			if(m_currentScene == m_menuScene)
			{
				m_menuScene->showOptionsView();
			}
		}
		setRunning(false);
		Timer timer;
		timer.start();
		double currentTime = 0.0;
		double dt = 0.0;
		double now = 0.0;
		while(device->run())
		{
			now = timer.getTimeSec();
			dt = now - currentTime;
			m_sceneLock.Lock();


			m_currentScene->AcquireLock();
			if(m_pendingInit)
			{
				//give it a chance to display the loading screen
				m_currentScene->update(static_cast<float>(dt));
				RenderManager::get().render();

				//do the init
				m_currentScene->init();
				m_currentScene->ReleaseLock();
				m_pendingInit = false;
			}
			else
			{
				RenderManager::get().render();
				m_currentScene->update(static_cast<float>(dt));
				m_currentScene->ReleaseLock();
			}
			m_sceneLock.Unlock();

			//Drain any lobby chat/system-message lines queued by other threads (main thread).
			flushLobbyChat();
			updateGameNotifications(static_cast<float>(dt));

			currentTime = now;

			/*
			if(m_pendingInit)
			{
				m_currentScene->AcquireLock();
				m_currentScene->update(static_cast<float>(dt));
				RenderManager::get().render();
				m_currentScene->ReleaseLock();
				m_currentScene->init();
				m_pendingInit = false;
				m_sceneLock.Unlock();
				currentTime = now;
			}
			else
			{
				m_currentScene->AcquireLock();
				RenderManager::get().render();
				m_currentScene->update(static_cast<float>(dt));
				m_currentScene->ReleaseLock();
				m_sceneLock.Unlock();
				currentTime = now;
			}
			*/


			/*
			now = timer.getTimeSec();
			if(device->isWindowActive())
			{
				dt = now - currentTime;
				m_sceneLock.Lock();
				RenderManager::get().render();
				m_currentScene->update(static_cast<float>(dt));	
				m_sceneLock.Unlock();
			}
			else device->yield();
			currentTime = now;
			*/
		}
		isFirstIteration = false;
	}
#ifdef _DEBUG
	m_config->getEventReceiver().unregisterEventListener(&logListener);
#endif
	m_terminated = true;
	return 0;
}

void System::setRunning(const bool r)
{
	m_running = r;
}

bool System::isRunning() const
{
	return m_running;
}

ConfigData* System::getConfig() const
{
	return m_config;
}

void System::registerEventListener(IEventListener* listener)
{
	m_config->getEventReceiver().registerEventListener(listener);
}

void System::unregisterEventListener(IEventListener* listener)
{
	m_config->getEventReceiver().unregisterEventListener(listener);
}

irr::scene::ISceneManager* System::getCurrentSceneManager()
{
	
	irr::scene::ISceneManager* mgr = NULL;
	m_sceneLock.Lock();
	mgr = m_currentScene->getSceneManager();
	m_sceneLock.Unlock();
	return mgr;
	
	//return m_currentScene->getSceneManager();
}

void System::toggle(const SCENE scene)
{
	if(scene > SCENE_FIRST_NOT_A_SCENE && scene < SCENE_LAST_NOT_A_SCENE)
	{
		//Don't let kill-feed/notification lines linger across a scene change.
		clearGameNotifications();
		m_sceneLock.Lock();
		if(m_currentScene)
		{
			m_currentScene->AcquireLock();
			m_currentScene->clean();
			m_currentScene->ReleaseLock();
		}
		NetworkController::get().unregisterFinalLANView();
		NetworkController::get().registerServerScene(NULL);
		switch(scene)
		{
			case SCENE_MENU:
			{
				m_currentScene = m_menuScene;
				break;
			}
			case SCENE_SINGLE:
			{
				m_currentScene = m_singlePlayerScene;
				m_config->registerKeybindings();
				break;
			}
			case SCENE_LAN_CLIENT:
			{
				m_currentScene = m_lanClientScene;
				m_config->registerKeybindings();
				//NetworkController::get().unregisterFinalLANView();
				break;
			}
			case SCENE_LAN_SERVER:
			{
				m_currentScene = m_lanServerScene;
				NetworkController::get().registerServerScene(m_lanServerScene);
				m_config->registerKeybindings();
				//NetworkController::get().unregisterFinalLANView();
				break;
			}
		}
		if(m_currentScene)
		{
			m_currentScene->setInitialized(false);
			m_pendingInit = true;
		}
		m_sceneLock.Unlock();
		/*
		if(m_currentScene)
			m_currentScene->init();
		*/
	}
}

void System::log(const char* msg) const
{
	if(DO_LOGGING && !m_terminated)
	{
		getDevice()->getLogger()->log(msg);
	}
}

void System::logw(const wchar_t* msg) const
{
	if(DO_LOGGING && !m_terminated)
	{
		getDevice()->getLogger()->log(msg);
	}
}

void System::appendLobbyChatLine(const wchar_t* line)
{
	if(m_terminated)
		return;
	//Just queue it here - this is often called from the network thread, and touching the
	//GUI off the main (render) thread corrupts Irrlicht's GUI tree and crashes. flushLobbyChat()
	//does the actual write on the main thread.
	m_chatQueueLock.Lock();
	m_pendingChatLines.push_back(irr::core::stringw(line));
	m_chatQueueLock.Unlock();
}

void System::flushLobbyChat()
{
	//Pull the queued lines out under the lock, then touch the GUI (main thread only).
	std::vector<core::stringw> lines;
	m_chatQueueLock.Lock();
	lines.swap(m_pendingChatLines);
	m_chatQueueLock.Unlock();

	if(lines.empty() || m_terminated)
		return;

	//If the chat box isn't present (we're not in a lobby), the lines are simply dropped -
	//the queue was already cleared by the swap above, so it can't grow without bound.
	gui::IGUIEditBox* chat = (gui::IGUIEditBox*)getDevice()->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_LANFINAL_CHAT_EDITBOX, true);
	if(chat)
	{
		core::stringw t(chat->getText());
		for(std::vector<core::stringw>::const_iterator it = lines.begin(); it != lines.end(); ++it)
		{
			t += L"\n";
			t += *it;
		}
		chat->setText(t.c_str());
	}
}

void System::pushGameNotification(const wchar_t* text)
{
	if(m_terminated)
		return;
	//Queue from whatever thread; the line is materialised on the main thread in
	//updateGameNotifications (drawing/aging the GUI off the main thread would race the renderer).
	m_notificationLock.Lock();
	m_pendingNotifications.push_back(irr::core::stringw(text));
	m_notificationLock.Unlock();
}

void System::updateGameNotifications(float dt)
{
	//Pull queued lines out under the lock, then work on the main-thread-only active list.
	std::vector<core::stringw> incoming;
	m_notificationLock.Lock();
	incoming.swap(m_pendingNotifications);
	m_notificationLock.Unlock();

	for(std::vector<core::stringw>::const_iterator it = incoming.begin(); it != incoming.end(); ++it)
	{
		GameNotification n;
		n.text = *it;
		n.remaining = 6.0f;//seconds on screen
		m_activeNotifications.push_back(n);
	}

	//Keep only the most recent few so a busy fight doesn't fill the screen.
	const size_t maxLines = 6;
	if(m_activeNotifications.size() > maxLines)
		m_activeNotifications.erase(m_activeNotifications.begin(), m_activeNotifications.begin() + (m_activeNotifications.size() - maxLines));

	for(std::vector<GameNotification>::iterator it = m_activeNotifications.begin(); it != m_activeNotifications.end(); )
	{
		it->remaining -= dt;
		if(it->remaining <= 0.0f)
			it = m_activeNotifications.erase(it);
		else
			++it;
	}
}

void System::drawGameNotifications()
{
	if(m_terminated || m_activeNotifications.empty())
		return;
	gui::IGUIFont* font = getDevice()->getGUIEnvironment()->getBuiltInFont();
	if(!font)
		return;

	const core::dimension2du screen = getDevice()->getVideoDriver()->getScreenSize();
	s32 y = 80;//start below the top-of-screen HUD text
	for(std::vector<GameNotification>::const_iterator it = m_activeNotifications.begin(); it != m_activeNotifications.end(); ++it)
	{
		u32 alpha = 255;
		if(it->remaining < 1.0f)
			alpha = (u32)(255.0f * it->remaining);//fade out over the last second

		const core::dimension2du textDim = font->getDimension(it->text.c_str());
		const s32 x = (s32)screen.Width - (s32)textDim.Width - 20;
		font->draw(it->text.c_str(), core::recti(x, y, x + (s32)textDim.Width, y + (s32)textDim.Height), video::SColor(alpha, 255, 230, 120));
		y += 18;
	}
}

void System::clearGameNotifications()
{
	m_notificationLock.Lock();
	m_pendingNotifications.clear();
	m_notificationLock.Unlock();
	m_activeNotifications.clear();
}

void System::updateSpaceObject(struct SpaceObjectNetworkInfo& info)
{
	//m_currentScene->AcquireLock();
	SpaceObject* obj = m_currentScene->GetSpaceObjectByID(info.id);
	if(obj && !obj->NeedsDeletion())
	{
		RenderObject* rendObj = obj->GetRenderObject();
		if (rendObj)
		{
			//Remote ships are eased toward the server transform (smooth motion); our own ship
			//and fast/short-lived objects (projectiles, warheads) snap straight to it - the own
			//ship for responsiveness, projectiles because smoothing would make them visibly lag.
			const bool isShip = (obj->ObjectMask & MASK_SHIP) != 0;
			const bool isOwnShip = isShip && m_currentScene->GetAgent() && m_currentScene->GetAgent()->GetSpaceObject() == obj;
			if(isShip && !isOwnShip)
			{
				obj->SetNetworkTarget(info.position, info.rotation);
			}
			else
			{
				rendObj->SetPosition(info.position);
				rendObj->SetRotation(info.rotation);
			}
		}

		obj->SetRemainingShields(info.shieldRemaining);
		obj->SetRemainingArmour(info.ArmourRemaining);
		obj->SetImpactTime(info.impactTime);
		obj->ShieldImpactVector(info.hitDirection); 
		/*
		if (info.ArmourRemaining < 0.001f)
		{
			if (rendObj) rendObj->GetSceneNode()->setVisible(false); 
		}
		*/
		switch(info.targetType)
		{
		default:
		case TARGET_NONE:
			obj->SetHardTarget(NULL);
			obj->SetSoftTarget(NULL);
			break;
		case TARGET_SOFT:
			obj->SetHardTarget(NULL);
			obj->SetSoftTarget(m_currentScene->GetSpaceObjectByID(info.targetID));
			break;
		case TARGET_HARD:
			obj->SetSoftTarget(m_currentScene->GetSpaceObjectByID(info.targetID)); // TODO - MIGHT NEED TO BE SET TO NULL 
			obj->SetHardTarget(m_currentScene->GetSpaceObjectByID(info.targetID));
			break;
		}

	}
	//m_currentScene->ReleaseLock();
}

SpaceObject* System::getSpaceObjectByID(const unsigned int id)
{
	return m_currentScene->GetSpaceObjectByID(id);
}

LogicScene* System::getCurrentScene() const
{
	return m_currentScene;
}

void System::feedDataToLANCLient(LANCreationHelper* data)
{
	m_lanClientScene->cacheData(data);
}

int main(int argc, char* argv[])
{
	srand(static_cast<unsigned int>(time(0)));//this downcast (and possible data loss) doesn't affect us since we're only using it to seed
	std::ofstream file;
	file.open("Output.txt");
	file.close();
	return System::get().run();
}


