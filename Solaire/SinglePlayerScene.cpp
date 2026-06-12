#include <irrlicht.h>

#include "SinglePlayerScene.h"
#include "System.h"
#include "LocalAgent.h"
#include "SpaceObject.h"
#include "PhysicsController.h"
#include "LogicConstants.h"
#include "ConfigData.h"
#include "InputConstants.h"
#include "GarbageCollector.h"
#include "GameLog.h"
#include "GUIConstants.h"
#include "RenderingConstants.h"

using namespace irr;

SinglePlayerScene::SinglePlayerScene() : LogicScene(false), m_smgr(NULL)
{
	
}

SinglePlayerScene::~SinglePlayerScene()
{
	clean();
}

int SinglePlayerScene::init()
{
	clean();
	GarbageCollector::Get().SetPause(false);
	m_smgr = System::get().getDevice()->getSceneManager()->createNewSceneManager(false);

	scene::ILightSceneNode* light = m_smgr->addLightSceneNode(0, core::vector3df(1.0f,0.0f,0.0f), video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 800.0f,LIGHT_ID);
	AcquireLock();

	setupScoreboard();

	SetAgent(new LocalAgent(L"Human", true));
	GetAgent()->SetMask(MASK_GROUP_1); 
	GetAgent()->SetParentScene(this);
	GetAgent()->SetID(1);
	GetAgent()->Init();	
	createPlayerScoreboardData(GetAgent());
	
	m_Dummy.SetMask(MASK_GROUP_1);
	m_Dummy.SetParentScene(this);
	m_Dummy.SetName("Karl");
	m_Dummy.SetID(2); 
	m_Dummy.Init();
	createPlayerScoreboardData(&m_Dummy);


	m_Dummy2.SetMask(MASK_GROUP_2);
	m_Dummy2.SetParentScene(this);
	m_Dummy2.SetName("Sicso");
	m_Dummy2.SetID(3); 
	m_Dummy2.Init();
	createPlayerScoreboardData(&m_Dummy2);

	m_Dummy3.SetMask(MASK_GROUP_2);
	m_Dummy3.SetParentScene(this);
	m_Dummy3.SetName("Other Sisco");
	m_Dummy3.SetID(4); 
	m_Dummy3.Init();
	createPlayerScoreboardData(&m_Dummy3);

	scene::ISceneNode *BoxNode = m_smgr->addSkyBoxSceneNode(System::get().getDevice()->getVideoDriver()->getTexture("GreenSpace_top3.png"), 
							System::get().getDevice()->getVideoDriver()->getTexture("GreenSpace_bottom4.png"), 
							System::get().getDevice()->getVideoDriver()->getTexture("GreenSpace_left2.png"), 
							System::get().getDevice()->getVideoDriver()->getTexture("GreenSpace_right1.png"), 
							System::get().getDevice()->getVideoDriver()->getTexture("GreenSpace_front5.png"), 
							System::get().getDevice()->getVideoDriver()->getTexture("GreenSpace_back6.png"));

	LogicScene::init();
	PhysicsController::Get().start();
	m_initialized = true;
	ReleaseLock(); 

	return 0;
}

int SinglePlayerScene::update(const float dt)
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
		for (auto i = m_SpaceObjectList.begin(); i != m_SpaceObjectList.end(); i++)
		{
			(*i).second->Update(dt);
		}

		GetAgent()->Update(dt);
		//ReleaseLock();

		//CleanupSpaceObjects();
		UpdateExplosions(dt);
	}
	return 0;
}

int SinglePlayerScene::clean()
{
	m_initialized = false;
	PhysicsController::Get().interrupt();
	m_Dummy.clean();
	m_Dummy2.clean();
	m_Dummy3.clean();
	GameLog::Get().Clean();

	LogicScene::clean();

	PhysicsController::Get().finish();
	GarbageCollector::Get().SetPause(true);
	GarbageCollector::Get().acquireLock();

	PhysicsController::Get().ForcedObjectsCleanup();
	ForcedSpaceObjectCleanup();

	GarbageCollector::Get().releaseLock();
	if(m_smgr)
	{
		m_smgr->clear();
		m_smgr->drop();
		m_smgr = NULL;
	}

	return 0;
}

irr::scene::ISceneManager* SinglePlayerScene::getSceneManager() const
{
	return m_smgr;
}
