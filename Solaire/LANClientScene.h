#pragma once
#include "LogicScene.h"
#include "LANClientLogicSceneGUIEventListener.h"
struct LANCreationHelper;

class LANClientScene : public LogicScene
{
public:
	LANClientScene();
	~LANClientScene();

	//PRE-condition: cacheData has been called with proper data
	int init();

	int update(const float dt);

	int clean();

	irr::scene::ISceneManager* getSceneManager() const;

	void cacheData(LANCreationHelper* data);//massive hack

protected:
	void toggleScoreboard();

private:

	LANClientLogicSceneGUIEventListener m_listener;
	irr::scene::ISceneManager* m_smgr;
	LANCreationHelper* m_cachedData;
};

