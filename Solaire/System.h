#pragma once
#include <vector>
#include <irrString.h>
#include "CSLock.h"

class ConfigData;
class LogicScene;
class MenuScene;
class IEventListener;
class LANClientScene;
class LANServerScene;
class SpaceObject;
struct LANCreationHelper;

struct SpaceObjectNetworkInfo;

namespace irr
{
	class IrrlichtDevice;

	namespace scene
	{
		class ISceneManager;
	}
}

enum SCENE
{
	SCENE_FIRST_NOT_A_SCENE,

	SCENE_MENU,
	SCENE_SINGLE,
	SCENE_LAN_CLIENT,
	SCENE_LAN_SERVER,

	SCENE_LAST_NOT_A_SCENE
};

class System
{
public:

	static System& get();

	
	irr::IrrlichtDevice* getDevice() const;


	int run();

	void setRunning(const bool r);

	bool isRunning() const;

	ConfigData* getConfig() const;

	void registerEventListener(IEventListener* listener);

	void unregisterEventListener(IEventListener* listener);

	irr::scene::ISceneManager* getCurrentSceneManager();

	void toggle(const SCENE scene);

	void log(const char* msg) const;

	void logw(const wchar_t* msg) const;

	//Queues a line to be shown in the LAN lobby chat box. Safe to call from any thread (e.g.
	//the network thread): the line is only written into the GUI on the main thread, in
	//flushLobbyChat(). Used for system notifications such as "X left the game".
	void appendLobbyChatLine(const wchar_t* line);

	//Main-thread only: drains the queued chat lines into the lobby chat box. Called once per
	//frame from run().
	void flushLobbyChat();

	//In-game notification overlay (kill feed, "X left the game", win banners). pushGameNotification
	//is safe from any thread (e.g. the network thread); updateGameNotifications ages them and
	//drawGameNotifications renders them - both main-thread only.
	void pushGameNotification(const wchar_t* text);
	void updateGameNotifications(float dt);
	void drawGameNotifications();
	void clearGameNotifications();

	void updateSpaceObject(struct SpaceObjectNetworkInfo& pos);

	SpaceObject* getSpaceObjectByID(const unsigned int id);

	LogicScene* getCurrentScene() const;

	void feedDataToLANCLient(LANCreationHelper* data);//massive hack
	
private:

	System();
	~System();

	void reinit();

	static System m_System;

	CSLock m_sceneLock;

	LogicScene* m_currentScene;
	MenuScene* m_menuScene;
	LogicScene* m_singlePlayerScene;
	LANClientScene* m_lanClientScene;
	LANServerScene* m_lanServerScene;


	ConfigData* m_config;

	//Lobby chat lines queued by other threads, drained on the main thread in flushLobbyChat().
	CSLock m_chatQueueLock;
	std::vector<irr::core::stringw> m_pendingChatLines;

	//In-game notification overlay. m_pendingNotifications is filled from any thread (guarded by
	//m_notificationLock); m_activeNotifications is main-thread only.
	struct GameNotification
	{
		irr::core::stringw text;
		float remaining;
	};
	CSLock m_notificationLock;
	std::vector<irr::core::stringw> m_pendingNotifications;
	std::vector<GameNotification> m_activeNotifications;

	bool m_running, m_terminated, m_pendingInit;

	System& operator= (const System& other);
	System(const System& other);
};

