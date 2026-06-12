#include <cmath>
#include <irrlicht.h>
#include <IrrlichtDevice.h>

#include "LogicScene.h"
#include "System.h"
#include "SpaceObject.h"
#include "Actuator.h"
#include "NetworkActuator.h"
#include "SpaceObjectShell.h"
#include "SpaceObjectNetworkInfo.h"
#include "SpaceObjectFactory.h"
#include "FactoryConstants.h"
#include "RenderObjectIncludes.h"
#include "LogicConstants.h"
#include "Agent.h"
#include "GUIConstants.h"
#include "InputConstants.h"
#include "ConfigData.h"
#include "KeyBindings.h"
#include "RenderingConstants.h"
#include "GameLog.h"

using namespace irr;
using std::pair;


std::vector<irr::core::stringc> LogicScene::m_pleaseWaitPictures(LogicScene::initPleaseWait());

std::vector<irr::core::stringc> LogicScene::initPleaseWait()
{
	std::vector<irr::core::stringc> hello;
	hello.push_back("pleasewait1.png");
	hello.push_back("pleasewait2.png");
	return hello;
}

LogicScene::LogicScene(bool networked) : m_pleaseWait(NULL), m_NextAvailableID (0), m_IsNetworked(networked), m_Agent(NULL), m_scoreboard(NULL), m_gameMenuTriggered(false), m_initialized(false), m_gameMenuInitialized(false), m_scoreboardTriggered(false), m_hasCentredMouse(false), 
	m_scoreboardCurrentY(0.0f), m_scoreboardCurrentTeamY(0.0f)
{
}

LogicScene::~LogicScene()
{
	clean();
	/*
	ForcedSpaceObjectCleanup();
	delete m_Agent;
	*/
}

void LogicScene::setInitialized(const bool b)
{
	m_initialized = b;
}

void LogicScene::AcquireLock()
{
	m_ListLock.Lock();
}
void LogicScene::ReleaseLock()
{
	m_ListLock.Unlock();
}

int LogicScene::init()
{
	System::get().registerEventListener(&m_guiListener);
	return 0;
}

int LogicScene::update(const float dt)
{
	//Apply anything the network thread queued (object creates/updates/deletes, actuator
	//input) here on the main thread, before the scene is read/rendered this frame.
	ApplyNetworkQueues();

	if(System::get().getConfig()->getKeybindings().isTriggered(GAME_MENU) && !m_gameMenuTriggered)
	{
		m_gameMenuTriggered = true;
		toggleGameMenu();
	}
	else if(!System::get().getConfig()->getKeybindings().isTriggered(GAME_MENU) && m_gameMenuTriggered)
	{
		m_gameMenuTriggered = false;
	}
	
	if(System::get().getConfig()->getKeybindings().isTriggered(ACTION_7) && !m_scoreboardTriggered)
	{
		m_scoreboardTriggered = true;
		toggleScoreboard();
	}
	else if(!System::get().getConfig()->getKeybindings().isTriggered(ACTION_7) && m_scoreboardTriggered)
	{
		m_scoreboardTriggered = false;
	}
	
	return 0;
}

int LogicScene::clean()
{
	//Drop anything the network thread queued but the main thread never applied, so we don't
	//leak it across a scene switch. The network client/server is paused before teardown.
	clearNetworkQueues();

	if(m_pleaseWait)
	{
		m_pleaseWait->remove();
		m_pleaseWait = NULL;
	}
	gui::IGUIElement* container = System::get().getDevice()->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_GAMEMENU_CONTAINER, false);
	if(container)
	{
		core::list<gui::IGUIElement*> list = container->getChildren();
		for(core::list<gui::IGUIElement*>::ConstIterator it = list.begin(); it != list.end(); ++it)
		{
			(*it)->remove();
		}
		container->remove();
	}
	if(m_scoreboard)
	{
		m_scoreboard->setVisible(false);
		core::list<gui::IGUIElement*> list = m_scoreboard->getChildren();
		for(core::list<gui::IGUIElement*>::ConstIterator it = list.begin(); it != list.end(); ++it)
		{
			(*it)->remove();
		}
		m_scoreboard->remove();
		m_scoreboard = NULL;
	}

	for(map<unsigned int, ScoreRowLabels*>::const_iterator it = m_playerScoreboardData.begin(); it != m_playerScoreboardData.end(); ++it)
	{
		//ScoreRowLabels* label = (*it).second;
		//delete label;
		delete it->second;
	}
	m_playerScoreboardData.clear();

	for(map<unsigned int, ScoreRowLabels*>::const_iterator it = m_teamScoreboardData.begin(); it != m_teamScoreboardData.end(); ++it)
	{
		//ScoreRowLabels* label = (*it).second;
		//delete label;
		delete it->second;
	}
	m_teamScoreboardData.clear();


	m_gameMenuInitialized = false;
	m_gameMenuTriggered = false;
	m_scoreboardTriggered = false;
	m_hasCentredMouse = false;
	System::get().unregisterEventListener(&m_guiListener);
	//ForcedSpaceObjectCleanup();
	delete m_Agent;
	m_Agent = NULL;
	return 0;
}

irr::scene::ISceneManager* LogicScene::getSceneManager() const
{
	return System::get().getDevice()->getSceneManager();
}

void LogicScene::toggleGameMenu()
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	if(!m_gameMenuInitialized)
	{
		video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
		core::dimension2d<u32> dim = driver->getScreenSize();
	
		float width = 0.85f;
		float height = 0.85f;

		float x = (1.0f - width) * 0.5f;
		float y = (1.0f - height) * 0.5f;

		core::rect<s32> r(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*y), static_cast<s32>(dim.Width*(x+width)), static_cast<s32>(dim.Height*(y+height)));
		gui::IGUITab* group = env->addTab(r, NULL, GUI_ID_GAMEMENU_CONTAINER);
		group->setDrawBackground(true);
		group->setBackgroundColor(video::SColor(128, 150, 128, 50));
		
		//!!! don't change any variables except those that are marked for it, unless you want to change the way things are aligned !!!

		const unsigned int numberOfButtons = 2;													//this can be changed
		const float buttonWidth = 0.2f;															//this can be changed
		const s32 buttonDY = 10;																//this can be changed

		const s32 buttonHeight = static_cast<s32>(dim.Height*(buttonWidth*0.5f));
		const s32 buttonX = static_cast<s32>((r.getWidth() - dim.Width*buttonWidth)*0.5f);
		const s32 totalButtonHeight = numberOfButtons * (buttonDY + buttonHeight) - buttonDY;

		s32 buttonY = static_cast<s32>((r.getHeight() - totalButtonHeight)*0.5f);
		gui::IGUIButton* continueButton = env->addButton(core::rect<s32>(buttonX, buttonY, buttonX+static_cast<s32>(dim.Width*buttonWidth), buttonY+buttonHeight), group, GUI_ID_GAMEMENU_CONTINUE_BUTTON, L"Resume", L"Continue the game");
		continueButton->setImage(driver->getTexture("TestButtonUp.tga"));
		continueButton->setPressedImage(driver->getTexture("TestButtonDown.tga"));
		continueButton->setScaleImage(true);
		continueButton->setUseAlphaChannel(true);
		continueButton->setDrawBorder(false);		

		buttonY += buttonDY + buttonHeight;
		gui::IGUIButton* exitButton = env->addButton(core::rect<s32>(buttonX, buttonY, buttonX+static_cast<s32>(dim.Width*buttonWidth), buttonY+buttonHeight), group, GUI_ID_GAMEMENU_EXIT_BUTTON, L"Quit", L"Exit to main menu");
		exitButton->setImage(driver->getTexture("TestButtonUp.tga"));
		exitButton->setPressedImage(driver->getTexture("TestButtonDown.tga"));
		exitButton->setScaleImage(true);
		exitButton->setUseAlphaChannel(true);
		exitButton->setDrawBorder(false);

		group->setVisible(false);
		m_gameMenuInitialized = true;
	}

	gui::IGUIElement* container = env->getRootGUIElement()->getElementFromId(GUI_ID_GAMEMENU_CONTAINER, false);
	if(container)
	{
		container->setVisible(!container->isVisible());
	}
}

void LogicScene::toggleScoreboard()
{
	if(m_scoreboard)
	{
		m_scoreboard->setVisible(!m_scoreboard->isVisible());
	}
}

static const unsigned int NUM_COLUMNS = 5;
static const float dx = 0.025f;
static const float dy = 0.04f;
static const float columns[NUM_COLUMNS][4] = {
	{0.025f, 0.025f, 0.3f, 0.035f},			//column1: X, Y, Width, Height
	{0.0f, 0.025f, 0.15f, 0.035f},			//column2: X, Y, Width, Height
	{0.0f, 0.025f, 0.15f, 0.035f},			//column3: X, Y, Width, Height
	{0.0f, 0.025f, 0.15f, 0.035f},			//column4: X, Y, Width, Height
	{0.0f, 0.025f, 0.15f, 0.035f},			//column5: X, Y, Width, Height
};


void LogicScene::setupScoreboard()
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = ::System::get().getDevice()->getVideoDriver();
	core::dimension2d<u32> dim = driver->getScreenSize();

	m_scoreboard = env->addTab(core::rect<s32>(static_cast<s32>(dim.Width*0.1f), static_cast<s32>(dim.Height*0.1f), static_cast<s32>(dim.Width*0.9f), static_cast<s32>(dim.Height*0.9f)), NULL, GUI_ID_SCOREBOARD_CONTAINER);
	m_scoreboard->setDrawBackground(true);
	m_scoreboard->setBackgroundColor(video::SColor(178, 150, 128, 50));

	core::dimension2d<s32> scoreDim = m_scoreboard->getAbsoluteClippingRect().getSize();


	wchar_t* headers[NUM_COLUMNS] = {L"Name", L"Score", L"Kills", L"Deaths", L"Accuracy"};

	float xStart = 0.0f;

	m_scoreboardCurrentY = 0.0f;//this is for the players list
	//header
	for(unsigned int i = 0; i < NUM_COLUMNS; ++i)
	{
		env->addStaticText(headers[i], 
			core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[i][0])), static_cast<s32>(scoreDim.Height*columns[i][1]), static_cast<s32>(scoreDim.Width*(xStart+columns[i][0]+columns[i][2])), static_cast<s32>(scoreDim.Height*(columns[i][1]+columns[i][3]))), 
			false, false, m_scoreboard);
		xStart+= columns[i][0] + columns[i][2]+dx;

		m_scoreboardCurrentY = std::max(m_scoreboardCurrentY, columns[i][1]+columns[i][3]);
	}
	m_scoreboardCurrentY+=dy;

	m_scoreboardCurrentTeamY = m_scoreboardCurrentY;

	m_scoreboardCurrentY+=6*dy;//save space for 4 teams plus some space

	m_scoreboard->setVisible(false);
}

void LogicScene::createPlayerScoreboardData(const unsigned int agentID, const wchar_t* name, const unsigned int mask)
{
	GameLog::Get().RegisterAgent(agentID, name, mask);

	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	core::dimension2d<s32> scoreDim = m_scoreboard->getAbsoluteClippingRect().getSize();

	const unsigned int groupID = (mask & GROUP_FILTER);

	vector3df colour(COLOUR_DEFAULT);
	switch(groupID)
	{
		case MASK_GROUP_1:
			colour = COLOUR_GROUP1;
			break;
		case MASK_GROUP_2:
			colour = COLOUR_GROUP2;
			break;
		case MASK_GROUP_3:
			colour = COLOUR_GROUP3;
			break;
		case MASK_GROUP_4:
			colour = COLOUR_GROUP4;
			break;
	}

	map<unsigned int, ScoreRowLabels*>::const_iterator it = m_playerScoreboardData.find(agentID);
	if(it == m_playerScoreboardData.end())
	{
		ScoreRowLabels* rowLabels = new ScoreRowLabels;
		float xStart = 0.0f;
		rowLabels->Name = env->addStaticText(name, 
				core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[0][0])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentY+columns[0][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[0][0]+columns[0][2])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentY+columns[0][1]+columns[0][3]))), 
				false, false, m_scoreboard);
		rowLabels->Name->setOverrideColor(video::SColor(255, static_cast<unsigned int>(255*colour.X), static_cast<unsigned int>(255*colour.Y), static_cast<unsigned int>(255*colour.Z)));
		xStart+= columns[0][0] + columns[0][2]+dx;

		rowLabels->Score = env->addStaticText(L"0", 
				core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[1][0])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentY+columns[1][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[1][0]+columns[1][2])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentY+columns[1][1]+columns[1][3]))), 
				false, false, m_scoreboard);
		rowLabels->Score->setOverrideColor(video::SColor(255, static_cast<unsigned int>(255*colour.X), static_cast<unsigned int>(255*colour.Y), static_cast<unsigned int>(255*colour.Z)));
		xStart+= columns[1][0] + columns[1][2]+dx;
		rowLabels->Kills = env->addStaticText(L"0", 
				core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[2][0])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentY+columns[2][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[2][0]+columns[2][2])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentY+columns[2][1]+columns[2][3]))), 
				false, false, m_scoreboard);
		rowLabels->Kills->setOverrideColor(video::SColor(255, static_cast<unsigned int>(255*colour.X), static_cast<unsigned int>(255*colour.Y), static_cast<unsigned int>(255*colour.Z)));
		xStart+= columns[2][0] + columns[2][2]+dx;
		rowLabels->Deaths = env->addStaticText(L"0", 
				core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[3][0])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentY+columns[3][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[3][0]+columns[3][2])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentY+columns[3][1]+columns[3][3]))), 
				false, false, m_scoreboard);
		rowLabels->Deaths->setOverrideColor(video::SColor(255, static_cast<unsigned int>(255*colour.X), static_cast<unsigned int>(255*colour.Y), static_cast<unsigned int>(255*colour.Z)));
		xStart+= columns[3][0] + columns[3][2]+dx;
		rowLabels->Accuracy = env->addStaticText(L"0.00%", 
				core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[4][0])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentY+columns[4][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[4][0]+columns[4][2])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentY+columns[4][1]+columns[4][3]))), 
				false, false, m_scoreboard);
		rowLabels->Accuracy->setOverrideColor(video::SColor(255, static_cast<unsigned int>(255*colour.X), static_cast<unsigned int>(255*colour.Y), static_cast<unsigned int>(255*colour.Z)));
		m_scoreboardCurrentY+=dy;

		m_playerScoreboardData.insert(std::pair<unsigned int, ScoreRowLabels*>(agentID, rowLabels));
	}


	map<unsigned int, ScoreRowLabels*>::const_iterator it2 = m_teamScoreboardData.find(groupID);
	if(it2 == m_teamScoreboardData.end())
	{
		float xStart = 0.0f;
		ScoreRowLabels* teamRowLabels = new ScoreRowLabels;
		wchar_t buffer[7];
		swprintf(buffer, L"Team %d", m_teamScoreboardData.size()+1);

		teamRowLabels->Name = env->addStaticText(buffer, 
				core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[0][0])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentTeamY+columns[0][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[0][0]+columns[0][2])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentTeamY+columns[0][1]+columns[0][3]))), 
				false, false, m_scoreboard);
		teamRowLabels->Name->setOverrideColor(video::SColor(255, static_cast<unsigned int>(255*colour.X), static_cast<unsigned int>(255*colour.Y), static_cast<unsigned int>(255*colour.Z)));
		xStart+= columns[0][0] + columns[0][2]+dx;

		teamRowLabels->Score = env->addStaticText(L"0", 
				core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[1][0])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentTeamY+columns[1][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[1][0]+columns[1][2])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentTeamY+columns[1][1]+columns[1][3]))), 
				false, false, m_scoreboard);
		teamRowLabels->Score->setOverrideColor(video::SColor(255, static_cast<unsigned int>(255*colour.X), static_cast<unsigned int>(255*colour.Y), static_cast<unsigned int>(255*colour.Z)));
		xStart+= columns[1][0] + columns[1][2]+dx;

		teamRowLabels->Kills = env->addStaticText(L"0", 
				core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[2][0])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentTeamY+columns[2][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[2][0]+columns[2][2])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentTeamY+columns[2][1]+columns[2][3]))), 
				false, false, m_scoreboard);
		teamRowLabels->Kills->setOverrideColor(video::SColor(255, static_cast<unsigned int>(255*colour.X), static_cast<unsigned int>(255*colour.Y), static_cast<unsigned int>(255*colour.Z)));
		xStart+= columns[2][0] + columns[2][2]+dx;

		teamRowLabels->Deaths = env->addStaticText(L"0", 
				core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[3][0])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentTeamY+columns[3][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[3][0]+columns[3][2])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentTeamY+columns[3][1]+columns[3][3]))), 
				false, false, m_scoreboard);
		teamRowLabels->Deaths->setOverrideColor(video::SColor(255, static_cast<unsigned int>(255*colour.X), static_cast<unsigned int>(255*colour.Y), static_cast<unsigned int>(255*colour.Z)));
		xStart+= columns[3][0] + columns[3][2]+dx;

		teamRowLabels->Accuracy = env->addStaticText(L"0.00%", 
				core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[4][0])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentTeamY+columns[4][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[4][0]+columns[4][2])), static_cast<s32>(scoreDim.Height*(m_scoreboardCurrentTeamY+columns[4][1]+columns[4][3]))), 
				false, false, m_scoreboard);
		teamRowLabels->Accuracy->setOverrideColor(video::SColor(255, static_cast<unsigned int>(255*colour.X), static_cast<unsigned int>(255*colour.Y), static_cast<unsigned int>(255*colour.Z)));
		m_scoreboardCurrentTeamY+=dy;

		m_teamScoreboardData.insert(std::pair<unsigned int, ScoreRowLabels*>(groupID, teamRowLabels));
	}
}

void LogicScene::createPlayerScoreboardData(Agent* agent)
{
	if(!agent)
		return;
	createPlayerScoreboardData(agent->GetID(), agent->GetName().c_str(), agent->GetMask());
}

void LogicScene::updateScoreboard()
{
	if(m_scoreboard)
	{	
		for(map<unsigned int, ScoreRowLabels*>::const_iterator it = m_playerScoreboardData.begin(); it != m_playerScoreboardData.end(); ++it)
		{
			ScoreRowLabels* labels = it->second;
			AgentLogData* player = GameLog::Get().GetData(it->first);

			labels->Score->setText(stringw(player->Score).c_str());
			labels->Kills->setText(stringw(player->Kills.size()).c_str());
			labels->Deaths->setText(stringw(player->Deaths.size()).c_str());
			float accuracy = 0.0f;
			if(player->Shots > 0)
			{
				accuracy = std::min(100.0f, 100*static_cast<float>(player->HitsGiven.size())/static_cast<float>(player->Shots));
			}
			wchar_t buffer[8];
			swprintf(buffer, L"%.2f%%", accuracy);
			labels->Accuracy->setText(buffer);
		}

		for(map<unsigned int, ScoreRowLabels*>::const_iterator it = m_teamScoreboardData.begin(); it != m_teamScoreboardData.end(); ++it)
		{
			unsigned int mask = it->first;
			ScoreRowLabels* labels = it->second;

			labels->Score->setText(stringw(GameLog::Get().GetGroupScore(mask)).c_str());
			labels->Kills->setText(stringw(GameLog::Get().GetGroupKills(mask)).c_str());
			labels->Deaths->setText(stringw(GameLog::Get().GetGroupDeaths(mask)).c_str());
			float acc = GameLog::Get().GetGroupAccuracy(mask);
			wchar_t buffer[8];
			swprintf(buffer, L"%.2f%%", acc);
			labels->Accuracy->setText(buffer);
		}
	}
}

void LogicScene::updateScoreboard(const std::vector<ScoreRow>& scores)
{
	if(m_scoreboard && m_scoreboard->isVisible())
	{	
		for(std::vector<ScoreRow>::const_iterator it = scores.begin(); it != scores.end(); ++it)
		{
			ScoreRow score = *it;
			ScoreRowLabels* labels = NULL;
			if(!score.IsTeam)
			{
				map<unsigned int, ScoreRowLabels*>::const_iterator scoreIt = m_playerScoreboardData.find(score.ID);
				if(scoreIt != m_playerScoreboardData.end())
				{
					labels = scoreIt->second;
				}
			}
			else
			{
				map<unsigned int, ScoreRowLabels*>::const_iterator scoreIt = m_teamScoreboardData.find(score.ID);
				if(scoreIt != m_teamScoreboardData.end())
				{
					labels = scoreIt->second;
				}
			}
			if(labels)
			{
				labels->Score->setText(stringw(score.Score).c_str());
				labels->Kills->setText(stringw(score.Kills).c_str());
				labels->Deaths->setText(stringw(score.Deaths).c_str());
				wchar_t buffer[8];
				swprintf(buffer, L"%.2f%%", score.Accuracy);
				labels->Accuracy->setText(buffer);
			}
		}
	}
}

unsigned int LogicScene::RequestSpaceObjectID()
{
	if (m_ReleasedIDs.empty())
	{
		++m_NextAvailableID;
		return m_NextAvailableID;
	}
	else
	{
		unsigned int result = m_ReleasedIDs.front();
		m_ReleasedIDs.pop();
		return result; 
	}
}

void LogicScene::ReleaseID(const unsigned int id)
{
	m_ReleasedIDs.push(id);
}

SpaceObject* LogicScene::GetSpaceObjectByID(const unsigned int id)
{
	map<unsigned int, SpaceObject*>::const_iterator it =  m_SpaceObjectList.find(id);
	if(it != m_SpaceObjectList.end())
	{
		return it->second;
	}
	return NULL;
}

/*
void LogicScene::DeleteSpaceObjectByID(const unsigned int id)
{
	map<unsigned int, SpaceObject*>::const_iterator it =  m_SpaceObjectList.find(id);
	if(it != m_SpaceObjectList.end())
	{
		getSceneManager()->getRootSceneNode()->removeChild(it->second->GetRenderObject()->GetSceneNode());
		ReleaseID(id);
		delete it->second;
		m_SpaceObjectList.erase(it);
	}
}
*/

void LogicScene::CleanupSpaceObjects()
{
	m_ListLock.Lock();
	for(map<unsigned int, SpaceObject*>::const_iterator it = m_SpaceObjectList.begin(); it != m_SpaceObjectList.end();)
	{
		SpaceObject* obj = it->second;
		if(obj->NeedsDeletion())
		{
			if(obj->GetRenderObject())
				getSceneManager()->getRootSceneNode()->removeChild(obj->GetRenderObject()->GetSceneNode());
			ReleaseID(it->first);
			delete obj;
			it = m_SpaceObjectList.erase(it);
		}
		else
			++it;
	}
	m_ListLock.Unlock();
}

void LogicScene::QueueNetworkCreation(const SpaceObjectShell& shell)
{
	m_NetQueueLock.Lock();
	m_PendingCreations.push_back(new SpaceObjectShell(shell));
	m_NetQueueLock.Unlock();
}

void LogicScene::QueueNetworkUpdate(const SpaceObjectNetworkInfo& info)
{
	m_NetQueueLock.Lock();
	m_PendingUpdates.push_back(new SpaceObjectNetworkInfo(info));
	m_NetQueueLock.Unlock();
}

void LogicScene::QueueNetworkDeletion(const unsigned int id)
{
	m_NetQueueLock.Lock();
	m_PendingDeletions.push_back(id);
	m_NetQueueLock.Unlock();
}

void LogicScene::QueueActuatorData(const ActuatorOutput& data)
{
	m_NetQueueLock.Lock();
	m_PendingActuatorData.push_back(new ActuatorOutput(data));
	m_NetQueueLock.Unlock();
}

void LogicScene::ApplyNetworkQueues()
{
	//Swap the queues out under the lock so it's held only briefly and we never run scene
	//code (which can be slow or re-enter the scene locks) while the network thread waits.
	std::vector<SpaceObjectShell*> creations;
	std::vector<SpaceObjectNetworkInfo*> updates;
	std::vector<unsigned int> deletions;
	std::vector<ActuatorOutput*> actuatorData;

	m_NetQueueLock.Lock();
	creations.swap(m_PendingCreations);
	updates.swap(m_PendingUpdates);
	deletions.swap(m_PendingDeletions);
	actuatorData.swap(m_PendingActuatorData);
	m_NetQueueLock.Unlock();

	//Order matters: create first so later updates/deletes that reference an object resolve.
	for(std::vector<SpaceObjectShell*>::const_iterator it = creations.begin(); it != creations.end(); ++it)
	{
		SpaceObjectFactory::Get().CreateObjectFromShell(this, *it);
		delete *it;
	}

	for(std::vector<SpaceObjectNetworkInfo*>::const_iterator it = updates.begin(); it != updates.end(); ++it)
	{
		System::get().updateSpaceObject(**it);
		delete *it;
	}

	for(std::vector<ActuatorOutput*>::const_iterator it = actuatorData.begin(); it != actuatorData.end(); ++it)
	{
		ActuatorOutput* data = *it;
		SpaceObject* obj = GetSpaceObjectByID(data->shipID);
		if(obj)
		{
			NetworkActuator* act = dynamic_cast<NetworkActuator*>(obj->GetActuator());
			if(act)
				act->feed(data->toData());
		}
		delete data;
	}

	//Deletes only flag the object; CleanupSpaceObjects (also main thread) frees it.
	for(std::vector<unsigned int>::const_iterator it = deletions.begin(); it != deletions.end(); ++it)
	{
		SpaceObject* obj = GetSpaceObjectByID(*it);
		if(obj)
			obj->FlagForDeletion();
	}
}

void LogicScene::clearNetworkQueues()
{
	m_NetQueueLock.Lock();
	for(std::vector<SpaceObjectShell*>::const_iterator it = m_PendingCreations.begin(); it != m_PendingCreations.end(); ++it)
		delete *it;
	m_PendingCreations.clear();
	for(std::vector<SpaceObjectNetworkInfo*>::const_iterator it = m_PendingUpdates.begin(); it != m_PendingUpdates.end(); ++it)
		delete *it;
	m_PendingUpdates.clear();
	for(std::vector<ActuatorOutput*>::const_iterator it = m_PendingActuatorData.begin(); it != m_PendingActuatorData.end(); ++it)
		delete *it;
	m_PendingActuatorData.clear();
	m_PendingDeletions.clear();
	m_NetQueueLock.Unlock();
}

void LogicScene::pleaseWait(const bool b)
{
	if(!m_pleaseWait)
	{
		video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
		core::dimension2du dim = driver->getScreenSize();
		m_pleaseWait = System::get().getDevice()->getGUIEnvironment()->addImage(core::rect<s32>(0, 0, dim.Width, dim.Height));
		unsigned int size = LogicScene::m_pleaseWaitPictures.size();
		m_pleaseWait->setImage(driver->getTexture(LogicScene::m_pleaseWaitPictures.at(rand()%size)));
		m_pleaseWait->setScaleImage(true);
	}
	m_pleaseWait->setVisible(b);
}

void LogicScene::ForcedSpaceObjectCleanup()
{
	m_ListLock.Lock();
	for(map<unsigned int, SpaceObject*>::const_iterator it = m_SpaceObjectList.begin(); it != m_SpaceObjectList.end(); it++)
	{
		getSceneManager()->getRootSceneNode()->removeChild(it->second->GetRenderObject()->GetSceneNode());
		delete it->second;
	}
	m_SpaceObjectList.clear();
	m_ListLock.Unlock();
}

void LogicScene::AddSpaceObject(unsigned int ID, SpaceObject* obj)
{
	std::pair<unsigned int, SpaceObject*> newObject;
	newObject.first = ID;
	newObject.second = obj;
	m_ListLock.Lock();
	m_SpaceObjectList.insert(newObject);
	m_ListLock.Unlock();
}

unsigned int LogicScene::AddSpaceObject(SpaceObject* obj)
{
	unsigned int ID = RequestSpaceObjectID();
	obj->ID = ID; 
	std::pair<unsigned int, SpaceObject*> newObject;
	newObject.first = ID;
	newObject.second = obj;
	m_ListLock.Lock();
	m_SpaceObjectList.insert(newObject);
	m_ListLock.Unlock();
	return ID; 
}

vector3df LogicScene::GetNextSpawnPoint(unsigned int mask)
{
	return m_Queue.GetSpawn(mask) * 0.6f;
}

const map<unsigned int, SpaceObject*>& LogicScene::GetObjectList() const 
{
	return m_SpaceObjectList;
}

bool LogicScene::IsNetworked()
{
	return m_IsNetworked;
}

void LogicScene::SetAgent(Agent* agent)
{
	m_Agent = agent; 
}

Agent* LogicScene::GetAgent()
{
	return m_Agent; 
}

void LogicScene::CreateExplosion(vector3df& pos, vector3df& rot, float size, unsigned int mask)
{		
	irr::video::SColor Colour(100, 0, 0, 0);
	unsigned int Group = (mask & GROUP_FILTER);

	switch (Group)
	{
	case MASK_GROUP_1:
		Colour.setBlue(255);
		Colour.setGreen(76);
		break;
	case MASK_GROUP_2:
		Colour.setRed(230);
		Colour.setGreen(51);
		break;
	case MASK_GROUP_3:
		Colour.setGreen(255);
		break;
	case MASK_GROUP_4:
		Colour.setRed(204);
		Colour.setBlue(204);
		break;
	default:
		break;
	}


	scene::IParticleSystemSceneNode* psBox =
            getSceneManager()->addParticleSystemSceneNode(false);
	scene::IParticleSystemSceneNode* psRing =
            getSceneManager()->addParticleSystemSceneNode(false);

	//psBox->addChild(psRing);
    scene::IParticleAffector* paf = psBox->createFadeOutParticleAffector(Colour);
    psBox->addAffector(paf);
	//psRing->addAffector(paf);
    paf->drop();

    scene::IParticleEmitter* emBox = psBox->createBoxEmitter(
            core::aabbox3d<f32>(-5,5,-5,5,1,5), // emitter size
            core::vector3df(0.0f,1.1f,0.0f),   // initial direction
            600,600,                             // emit rate
            video::SColor(0,255,255,255),       // darkest color
            video::SColor(0,255,255,255),       // brightest color
            800,1000,360,                         // min and max age, angle
            core::dimension2df(40.f,40.f),         // min size
            core::dimension2df(60.f,60.f));        // max size
	//scene::IParticleEmitter* em = ps->createSphereEmitter(vector3df(0.0f, 0.0f, 0.0f), 100.0f);
 
	psBox->setEmitter(emBox); // this grabs the emitter
    emBox->drop(); // so we can drop it here without deleting it



	psBox->setPosition(pos);
    psBox->setScale(core::vector3df(3,3,3));
    psBox->setMaterialFlag(video::EMF_LIGHTING, false);
    psBox->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	psBox->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("Spark.tga"));
    psBox->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);


		
	scene::IParticleRingEmitter* emRing = psRing->createRingEmitter(vector3df(0.0f, 0.0f, 0.0f), 1.0f, 0.001f, // Centre, radius, thickness
															core::vector3df(0.0f,0.0f,0.0f),   // initial direction
															10000,10000,                             // emit rate
															video::SColor(255,255,255,255),       // darkest color
															video::SColor(255,255,255,255),       // brightest color
															50,50,0,                         // min and max age, angle
															core::dimension2df(40.f,40.f),         // min size
															core::dimension2df(40.f,40.f));        // max size

	psRing->setEmitter(emRing);
	emRing->drop();

	psRing->setRotation(rot);
	psRing->setPosition(pos);
    psRing->setScale(core::vector3df(3,3,3));
    psRing->setMaterialFlag(video::EMF_LIGHTING, false);
    psRing->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	psRing->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("Spark.tga"));
    psRing->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

	ExplosionContainer cont;
	cont.Timer = 3.0f;
	cont.BoxNode = psBox;
	cont.RingNode = psRing; 
	cont.BoxActive = true;
	cont.RingActive = true; 
	m_Explosions.push_back(cont);
}

void LogicScene::UpdateExplosions(const float dt)
{
	for (auto i = m_Explosions.begin(); i != m_Explosions.end();)
	{
		(*i).Timer -= dt;

		if ((*i).Timer < 0.0f)
		{
			(*i).BoxNode->remove();
			(*i).RingNode->remove();
			i = m_Explosions.erase(i);
		}
		else
		{
			if ((*i).Timer < 2.5f && (*i).BoxActive)
			{
				(*i).BoxNode->setEmitter(0);
				(*i).BoxActive = false;
			}

			if((*i).Timer < 2.0f && (*i).RingActive)
			{
				(*i).RingNode->setEmitter(0);
				(*i).RingActive = false;
			}
			else if ((*i).RingActive)
			{
				(*i).RingNode->setScale((*i).RingNode->getScale() + 600.0f * dt);
				
			}

			i++;
		}
	}
}