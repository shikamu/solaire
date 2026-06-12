#include <cassert>
#include <algorithm>

#include <irrlicht.h>

#include "LANServerScene.h"
#include "System.h"
#include "SpaceObject.h"
#include "PhysicsController.h"
#include "NetworkController.h"
#include "LANServer.h"
#include "TCPPacketFactory.h"
#include "TCPPacket.h"
#include "SpaceObjectNetworkInfo.h"
#include "LogicConstants.h"
#include "ServerLocalAgent.h"
#include "SpaceObjectFactory.h"
#include "RemoteAgent.h"
#include "LANGameCreationHelper.h"
#include "LockPointer.h"
#include "GarbageCollector.h"
#include "ConfigData.h"
#include "GameLog.h"

using namespace irr;


LANServerScene::LANServerScene() : LogicScene(true), m_smgr(NULL), unregisterServerOnClean(false)
{
}

LANServerScene::~LANServerScene()
{
	clean();
}

int LANServerScene::init()
{
	AcquireLock();
	unregisterServerOnClean = false;
	clean();
	unregisterServerOnClean = true;

	GarbageCollector::Get().SetPause(false);
	m_smgr = System::get().getDevice()->getSceneManager()->createNewSceneManager(false);
	m_smgr->addLightSceneNode(0, core::vector3df(0,10000000.0f,0), video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 800.0f,LIGHT_ID);	

	LANServer* server = NetworkController::get().getServer();
	assert(server);
	server->setPaused(true);
	Sleep(200);
	
	setupScoreboard();

	//AcquireLock();
	//create server's agent, which will create the ship in the process
	SetAgent(new ServerLocalAgent(server->getServerNickname()));
	GetAgent()->SetMask(server->getTeamForName(server->getServerNickname()));
	GetAgent()->SetParentScene(this);	
	GetAgent()->SetID(server->getAvailableAgentID());
	GetAgent()->Init();
	createPlayerScoreboardData(GetAgent());

	std::map<SOCKET, LANCreationAssociationHelper> helper;
	
	std::map<SOCKET, irr::core::stringw> players = server->getPlayers();
	for(std::map<SOCKET, irr::core::stringw>::const_iterator it = players.begin(); it != players.end(); ++it) //note that the server is paused so the players list shouldn't change at this point, therefore the following loop can be considered safe
	{
		const unsigned int agentID = server->getAvailableAgentID();
		const unsigned int group = server->getTeamForName(it->second.c_str());
		core::vector3df pos = GetNextSpawnPoint(group);
		const unsigned int shipID = SpaceObjectFactory::Get().CreateShip(this, ACT_NET_SERVER, agentID, it->second, group | MASK_SHIP, pos, pos, false);

		RemoteAgent* agent = new RemoteAgent;
		agent->SetMask(group | MASK_SHIP);
		agent->SetParentScene(this);
		agent->SetName(it->second);
		agent->SetID(agentID); 
		agent->SetSpaceObject(GetSpaceObjectByID(shipID));
		agent->GetSpaceObject()->SetName(it->second);
		agent->Init();
		createPlayerScoreboardData(agent);

		addAgent(agent);
		server->insertAgent(it->first, agentID);
		
		LANCreationAssociationHelper h;
		h.agentID = agentID;
		h.shipID = shipID;
		h.mask = group;
		helper[it->first] = h;
	}

	std::vector<SpaceObjectShell*> allShells;
	std::vector<LockPointer<SpaceObjectShell>*> allFromServer = server->getAllShells();
	for(std::vector<LockPointer<SpaceObjectShell>*>::const_iterator it = allFromServer.begin(); it != allFromServer.end(); ++it)
	{
		allShells.push_back((*it)->GetUnlockedPointer());
	}

	LANCreationHelper creation;
	creation.allShips = allShells;
	
	for(std::map<SOCKET, LANCreationAssociationHelper>::const_iterator it = helper.begin(); it != helper.end(); ++it)
	{
		creation.agentID = it->second.agentID;
		creation.shipID = it->second.shipID;
		creation.mask = it->second.mask;
		TCPPacket* packet = TCPPacketFactory::get().createPacket(CREATE_INITIAL_SHIPS_AND_AGENT, &creation, 0);
		if(packet)
		{
			packet->send(it->first);
			delete packet;
		}
	}
	//ReleaseLock();
	server->setPaused(false);

	irr::video::IVideoDriver* video = System::get().getDevice()->getVideoDriver();
	m_smgr->addSkyBoxSceneNode(video->getTexture("GreenSpace_top3.png"), 
							video->getTexture("GreenSpace_bottom4.png"), 
							video->getTexture("GreenSpace_left2.png"), 
							video->getTexture("GreenSpace_right1.png"), 
							video->getTexture("GreenSpace_front5.png"), 
							video->getTexture("GreenSpace_back6.png"));

	//LogicScene::init();
	System::get().registerEventListener(&m_listener);
	PhysicsController::Get().start();
	m_initialized = true;
	ReleaseLock();
	return 0;
}

int LANServerScene::update(const float dt)
{
	pleaseWait(!m_initialized);
	if(m_initialized)
	{
		LogicScene::update(dt);
		if(!m_hasCentredMouse)
		{
			System::get().getDevice()->getCursorControl()->setPosition(0.5f, 0.5f);
			m_hasCentredMouse = true;
		}

		System::get().getConfig()->getKeybindings().update(dt);

		if(m_scoreboard && m_scoreboard->isVisible())
		{
			updateScoreboard();
		}


		//AcquireLock();
		for(map<unsigned int, SpaceObject*>::iterator it = m_SpaceObjectList.begin(); it != m_SpaceObjectList.end(); ++it)
		{
			(*it).second->Update(dt);
		}

		Agent* agent = GetAgent();
		if(agent)
		{
			agent->Update(dt);
		}

		UpdateExplosions(dt);

		//ReleaseLock();

		//CleanupSpaceObjects();
	}
	return 0;
}

int LANServerScene::clean()
{
	m_initialized = false;
	m_hasCentredMouse = false;
	PhysicsController::Get().interrupt();

	for(std::map<unsigned int, Agent*>::const_iterator it = m_playerAgents.begin(); it != m_playerAgents.end(); ++it)
	{
		delete it->second;
	}
	m_playerAgents.clear();

	GameLog::Get().Clean();
	LogicScene::clean();
	System::get().unregisterEventListener(&m_listener);

	PhysicsController::Get().finish();
	GarbageCollector::Get().SetPause(true);
	GarbageCollector::Get().acquireLock();

	PhysicsController::Get().ForcedObjectsCleanup();
	ForcedSpaceObjectCleanup();

	GarbageCollector::Get().releaseLock();

	if(unregisterServerOnClean)
		NetworkController::get().unregisterLANServer();

	if(m_smgr)
	{
		m_smgr->clear();
		m_smgr->drop();
		m_smgr = NULL;
	}
	return 0;
}

irr::scene::ISceneManager* LANServerScene::getSceneManager() const
{
	return m_smgr;
}

void LANServerScene::addAgent(Agent* agent)
{
	if(agent)
		m_playerAgents.insert(std::pair<unsigned int, Agent*>(agent->GetID(), agent));
}

void LANServerScene::replaceAgent(const unsigned int agentID)
{
	std::map<unsigned int, Agent*>::const_iterator it = m_playerAgents.find(agentID);
	if(it != m_playerAgents.end() && it->second)
	{
		//A disconnecting player's agent may not currently own a ship (e.g. it died and
		//hasn't respawned), so the SpaceObject and its actuator can legitimately be NULL.
		SpaceObject* obj = it->second->GetSpaceObject();
		if(obj && obj->GetActuator())
			obj->GetActuator()->SetNeedReplacement(true);
	}
}
