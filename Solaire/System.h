#pragma once
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

	bool m_running, m_terminated, m_pendingInit;

	System& operator= (const System& other);
	System(const System& other);
};

