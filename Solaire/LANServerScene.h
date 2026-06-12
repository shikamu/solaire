#pragma once
#include <map>
#include "LogicScene.h"
#include "LANServerLogicSceneGUIEventListener.h"

class LANServerScene : public LogicScene
{
public:
	LANServerScene();
	~LANServerScene();

	int init();

	int update(const float dt);

	int clean();

	irr::scene::ISceneManager* getSceneManager() const;

	void addAgent(Agent* agent);

	void replaceAgent(const unsigned int agentID);

private:

	std::map<unsigned int, Agent*> m_playerAgents;
	LANServerLogicSceneGUIEventListener m_listener;

	irr::scene::ISceneManager* m_smgr;

	bool unregisterServerOnClean;
};

