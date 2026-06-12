#include <cassert>
#include <irrlicht.h>

#include "LANClientScene.h"
#include "System.h"
#include "SpaceObject.h"
#include "LogicConstants.h"
#include "LocalClientAgent.h"
#include "LANGameCreationHelper.h"
#include "SpaceObjectFactory.h"
#include "SpaceObjectShell.h"
#include "ConfigData.h"
#include "TCPPacket.h"
#include "TCPPacketFactory.h"
#include "NetworkController.h"
#include "GameLog.h"

using namespace irr;

LANClientScene::LANClientScene() : LogicScene(false), m_smgr(NULL), m_cachedData(NULL)
{
}

LANClientScene::~LANClientScene()
{
	clean();
	
	//clean only deletes if the data has been consumed (i.e. it's flagged as dirty)
	if(m_cachedData)
	{
		delete m_cachedData;
	}
}

int LANClientScene::init()
{
	AcquireLock();
	clean();

	m_smgr = System::get().getDevice()->getSceneManager()->createNewSceneManager(false);
	m_smgr->addLightSceneNode(0, core::vector3df(0,10000000.0f,0), video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 800.0f,LIGHT_ID);

	setupScoreboard();
	if(m_cachedData)
	{
		assert(!m_cachedData->dirty);
		//if it's dirty, then something's wrong and we can't use it because the spaceobjectshell would already have been deleted
		
		//AcquireLock();
		LocalClientAgent* agent = new LocalClientAgent;
		agent->SetMask(m_cachedData->mask); 
		agent->SetParentScene(this);
		agent->SetID(m_cachedData->agentID);
		SetAgent(agent);

		for(std::vector<SpaceObjectShell*>::const_iterator it = m_cachedData->allShips.begin(); it != m_cachedData->allShips.end(); ++it)
		{
			bool needToInitAgent = false;
			SpaceObjectShell* shell = *it;
			//create the agent before we possibly create our ship because then, if the agent exists, the CreateObjectFromShell function will set up the actuator automatically for us and init the agent
			if(shell->ID == m_cachedData->shipID)
			{
				agent->SetName(shell->Name);
			}

			createPlayerScoreboardData(shell->AgentID, shell->Name.c_str(), shell->Mask);

			SpaceObjectFactory::Get().CreateObjectFromShell(this, shell);
			delete shell;
		}

		//ReleaseLock();
		m_cachedData->dirty = true;
	}


	irr::video::IVideoDriver* video = System::get().getDevice()->getVideoDriver();
	m_smgr->addSkyBoxSceneNode(video->getTexture("GreenSpace_top3.png"), 
							video->getTexture("GreenSpace_bottom4.png"), 
							video->getTexture("GreenSpace_left2.png"), 
							video->getTexture("GreenSpace_right1.png"), 
							video->getTexture("GreenSpace_front5.png"), 
							video->getTexture("GreenSpace_back6.png"));

	System::get().registerEventListener(&m_listener);
	//LogicScene::init();
	ReleaseLock();
	m_initialized = true;
	return 0;
}

int LANClientScene::update(const float dt)
{
	pleaseWait(!m_initialized);
	if(m_initialized)
	{
		LogicScene::update(dt);

		//LogicScene::update just applied the queued network updates (which set each remote
		//ship's target transform). Now ease every ship toward its target for smooth motion.
		//Objects without a network target (our own ship, projectiles) are left untouched.
		for(std::map<unsigned int, SpaceObject*>::const_iterator it = m_SpaceObjectList.begin(); it != m_SpaceObjectList.end(); ++it)
		{
			it->second->InterpolateToNetworkTarget(dt);
		}

		if(!m_hasCentredMouse)
		{
			System::get().getDevice()->getCursorControl()->setPosition(0.5f, 0.5f);
			m_hasCentredMouse = true;
		}
		System::get().getConfig()->getKeybindings().update(dt);

		//AcquireLock();
		
		if(GetAgent())
			GetAgent()->Update(dt);
		
		UpdateExplosions(dt);
		//ReleaseLock();
		CleanupSpaceObjects();
	}
	return 0;
}

int LANClientScene::clean()
{
	m_initialized = false;
	if(m_cachedData && m_cachedData->dirty)
	{
		delete m_cachedData;
		m_cachedData = NULL;
	}

	GameLog::Get().Clean();

	LogicScene::clean();
	System::get().unregisterEventListener(&m_listener);

	ForcedSpaceObjectCleanup();

	if(m_smgr)
	{
		m_smgr->clear();
		m_smgr->drop();
		m_smgr = NULL;
	}
	m_gameMenuTriggered = false;

	return 0;
}

irr::scene::ISceneManager* LANClientScene::getSceneManager() const
{
	return m_smgr;
}

void LANClientScene::cacheData(LANCreationHelper* data)
{
	if(m_cachedData)
	{
		delete m_cachedData;
	}
	m_cachedData = data;
	m_cachedData->dirty = false;
}

void LANClientScene::toggleScoreboard()
{
	LogicScene::toggleScoreboard();
	TCPPacket* packet = TCPPacketFactory::get().createPacket(TOGGLE_REQUEST_SCORE_UPDATES, NULL, 0);
	if(packet)
	{
		NetworkController::get().sendPacket(packet);
		delete packet;
	}
}
