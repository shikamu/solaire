#pragma once 
#include <map>
#include <irrString.h>
#include <vector>
#include "ScoreUpdateHelper.h"

using std::map;
using std::vector; 
using irr::core::stringw;

class Agent;

class AgentLogData
{
public:
	AgentLogData();
	AgentLogData(const stringw& name, unsigned int id, unsigned int mask);
	~AgentLogData();
	unsigned int ID;
	unsigned int GroupID; 
	stringw Name; 
	int Score;
	unsigned int Shots;
	vector<unsigned int> Kills;				//contains the IDs of the agent who this agent killed
	vector<unsigned int> Deaths;			//contains the IDs of the agent who killed this agent
	vector<unsigned int> HitsGiven;			//contains the IDs of the agents this agent has it
	vector<unsigned int> HitsReceived;		//contains the IDs of agents who hit me
	map<unsigned int, float> DamageLog;		//LOLOLOLOL
	void Clean();

	ScoreRow toScoreRow();
};

class GroupLogData
{
public:
	GroupLogData(const unsigned int groupID);

	int Score;
	unsigned int Shots;
	unsigned int Kills;
	unsigned int Deaths;
	unsigned int HitsGiven;

	void clean();

	ScoreRow toScoreRow();


private:

	const unsigned int m_groupID;
};

class GameLog
{
private:
	static GameLog m_Log; 
	GameLog();
	~GameLog(); 
	map<unsigned int, AgentLogData*> m_Data;
	map<unsigned int, GroupLogData*> m_GroupData;
	bool m_matchOver;//latched true once a team reaches the kill limit; reset in Clean()
	//map<unsigned int, int> m_GroupScore; 
	void LogGroupScore(unsigned int groupID, int value);
	void LogGroupDeath(unsigned int groupID);
	void LogGroupKill(unsigned int groupID);
	void LogGroupShot(unsigned int groupID);
	void LogGroupHit(unsigned int groupID);

public:
	static GameLog& Get(); 
	void Clean();

	void RegisterAgent(Agent* agent);
	void RegisterAgent(const unsigned int agentID, const wchar_t* name, const unsigned int mask);
	AgentLogData* GetData(unsigned int agentid);



	map<unsigned int, AgentLogData*> getAllPlayerData()
	{
		return m_Data; 
	}

	map<unsigned int, GroupLogData*> getAllTeamData()
	{
		return m_GroupData; 
	}


	void LogShot(unsigned int agentID);
	//Records the death and returns the killer's agent ID (0 if it was a suicide / unknown).
	unsigned int LogDeath(unsigned int agentID);
	void LogHit(unsigned int sourceID, unsigned int targetID);

	//Returns the group mask of a team that has just reached MATCH_KILL_LIMIT (once per match),
	//or 0 if no team has won yet. Latches so it only fires a single time.
	unsigned int CheckForWinner();
	//void LogKill(unsigned int sourceID, unsigned int targetID);
	int GetGroupScore(unsigned int groupID);
	int GetGroupKills(unsigned int groupID);
	int GetGroupDeaths(unsigned int groupID);
	float GetGroupAccuracy(unsigned int groupID);
};
