#pragma once
#include <irrString.h>
#include <queue>
#include <map>
#include <vector>
#include <vector3d.h>

#include "LogicSceneGUIEventListener.h"
#include "CSLock.h"
#include "SpawnQueue.h"
#include "ScoreUpdateHelper.h"

class SpaceObject; 
class Actuator; 
class Agent; 

using std::queue;
using std::map;
using std::vector;
using irr::core::vector3df;

namespace irr
{
	namespace scene
	{
		class ISceneManager;
		class IParticleSystemSceneNode;
	}
	namespace gui
	{
		class IGUIImage;
		class IGUIStaticText;
		class IGUITab;
	}
}
struct ExplosionContainer
{
	float Timer; 
	irr::scene::IParticleSystemSceneNode* BoxNode;
	irr::scene::IParticleSystemSceneNode* RingNode;
	bool BoxActive; 
	bool RingActive;
};

struct ScoreRowLabels
{
	irr::gui::IGUIStaticText* Name, *Score, *Kills, *Deaths, *Accuracy;

	ScoreRowLabels() : Name(NULL), Score(NULL), Kills(NULL), Deaths(NULL), Accuracy(NULL)
	{

	}
};

class LogicScene
{
private:

	LogicSceneGUIEventListener m_guiListener;

	static std::vector<irr::core::stringc> m_pleaseWaitPictures;

	//add new "please wait" screens in this function
	static std::vector<irr::core::stringc> initPleaseWait();
	
	irr::gui::IGUIImage* m_pleaseWait;
	unsigned int m_NextAvailableID; 
	queue<unsigned int> m_ReleasedIDs; 
	bool m_IsNetworked; 
	Agent* m_Agent;
	CSLock m_ListLock;
	SpawnQueue m_Queue; 
	vector<ExplosionContainer> m_Explosions;
	float m_scoreboardCurrentY, m_scoreboardCurrentTeamY;

protected:
	void ForcedSpaceObjectCleanup();
	void pleaseWait(const bool b);
	void toggleGameMenu();
	virtual void toggleScoreboard();
	
	void setupScoreboard();
	void createPlayerScoreboardData(const unsigned int agentID, const wchar_t* playerName, const unsigned int mask);
	void createPlayerScoreboardData(Agent* agent);
	void updateScoreboard();
	
	map<unsigned int, SpaceObject*> m_SpaceObjectList;

	map<unsigned int, ScoreRowLabels*> m_playerScoreboardData;	//the unsigned int is the agent of the player
	map<unsigned int, ScoreRowLabels*> m_teamScoreboardData;	//the unsigned int is the MASK_GROUP<X> of the team
	irr::gui::IGUITab* m_scoreboard;

	bool m_gameMenuTriggered, m_initialized, m_gameMenuInitialized, m_scoreboardTriggered, m_hasCentredMouse;

public:

	LogicScene(bool networked = false);
	virtual ~LogicScene();

	void updateScoreboard(const std::vector<ScoreRow>& scores);

	void setInitialized(const bool b);

	void AcquireLock();
	void ReleaseLock(); 

	virtual int init();

	virtual int update(const float dt);

	virtual int clean();

	virtual irr::scene::ISceneManager* getSceneManager() const;

	unsigned int RequestSpaceObjectID();
	void ReleaseID(const unsigned int);
	SpaceObject* GetSpaceObjectByID(const unsigned int id);
	void AddSpaceObject(unsigned int ID, SpaceObject* obj);
	unsigned int AddSpaceObject(SpaceObject* obj);
	//void DeleteSpaceObjectByID(const unsigned int id);
	void CleanupSpaceObjects();
	vector3df GetNextSpawnPoint(unsigned int mask);
	const map<unsigned int, SpaceObject*>& GetObjectList() const;
	bool IsNetworked();
	void SetAgent(Agent* agent);
	Agent* GetAgent();

	void CreateExplosion(vector3df& pos, vector3df& rot, float size, unsigned int mask);
	void UpdateExplosions(const float dt);
};

