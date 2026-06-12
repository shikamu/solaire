#include <cmath>

#include "GameLog.h"
#include "Agent.h"
#include "LogicConstants.h"


using std::pair;

AgentLogData::AgentLogData() : ID (0), Name (""), Shots (0), Kills (0), Deaths (0), Score (0), GroupID (0)
{
}
AgentLogData::AgentLogData(const stringw& name, unsigned int id, unsigned int mask) : ID (id), Name (name), Shots (0), Kills (0), Deaths (0), Score (0), GroupID (GROUP_FILTER & mask)
{
}
AgentLogData::~AgentLogData()
{
	HitsGiven.clear();
	HitsReceived.clear();
}

void AgentLogData::Clean()
{
	Shots = 0;
	Score = 0;
	Kills.clear();
	Deaths.clear();
	HitsGiven.clear();
	HitsReceived.clear();
}

ScoreRow AgentLogData::toScoreRow()
{
	float accuracy = 0.0f;
	if(Shots > 0)
	{
		accuracy = std::min(100.0f, 100*static_cast<float>(HitsGiven.size())/static_cast<float>(Shots));
	}
	return ScoreRow(ID, Score, Kills.size(), Deaths.size(), accuracy, 0);
}

GroupLogData::GroupLogData(const unsigned int id) : m_groupID(id), Score(0), Shots(0), Kills(0), Deaths(0), HitsGiven(0)
{

}

void GroupLogData::clean()
{
	Score = 0;
	Shots = 0;
	Kills = 0;
	Deaths = 0;
	HitsGiven = 0;
}

ScoreRow GroupLogData::toScoreRow()
{
	float accuracy = 0.0f;
	if(Shots > 0)
	{
		accuracy = std::min(100.0f, 100*static_cast<float>(HitsGiven)/static_cast<float>(Shots));
	}
	return ScoreRow(m_groupID, Score, Kills, Deaths, accuracy, 1);
}

GameLog GameLog::m_Log;

GameLog& GameLog::Get()
{
	return m_Log;
}
GameLog::GameLog()
{
	m_GroupData.insert(pair<unsigned int, GroupLogData*>(MASK_GROUP_1, new GroupLogData(MASK_GROUP_1)));
	m_GroupData.insert(pair<unsigned int, GroupLogData*>(MASK_GROUP_2, new GroupLogData(MASK_GROUP_2)));
	m_GroupData.insert(pair<unsigned int, GroupLogData*>(MASK_GROUP_3, new GroupLogData(MASK_GROUP_3)));
	m_GroupData.insert(pair<unsigned int, GroupLogData*>(MASK_GROUP_4, new GroupLogData(MASK_GROUP_4)));
}

GameLog::~GameLog()
{
	Clean();

	//have to put this deletion here because the groups are only created in the constructor of the GameLog (i.e. only once ever) so we reuse them and only delete them when the log is no longer in use, i.e. in this destructor
	for (auto i = m_GroupData.begin(); i != m_GroupData.end(); i++)
	{
		delete i->second; 
	}
	m_GroupData.clear();

}

void GameLog::Clean()
{
	for (auto i = m_Data.begin(); i != m_Data.end(); i++)
	{
		delete i->second; 
	}
	m_Data.clear();
	for (auto i = m_GroupData.begin(); i != m_GroupData.end(); i++)
	{
		i->second->clean();
	}
}

void GameLog::LogGroupScore(unsigned int groupID, int value)
{
	map<unsigned int, GroupLogData*>::const_iterator it = m_GroupData.find(groupID);
	if(it != m_GroupData.end())
	{
		it->second->Score += value;
	}
}

void GameLog::LogGroupDeath(unsigned int groupID)
{
	map<unsigned int, GroupLogData*>::const_iterator it = m_GroupData.find(groupID);
	if(it != m_GroupData.end())
	{
		it->second->Deaths++;
	}
}

void GameLog::LogGroupKill(unsigned int groupID)
{
	map<unsigned int, GroupLogData*>::const_iterator it = m_GroupData.find(groupID);
	if(it != m_GroupData.end())
	{
		it->second->Kills++;
	}
}

void GameLog::LogGroupShot(unsigned int groupID)
{
	map<unsigned int, GroupLogData*>::const_iterator it = m_GroupData.find(groupID);
	if(it != m_GroupData.end())
	{
		it->second->Shots++;
	}
}

void GameLog::LogGroupHit(unsigned int groupID)
{
	map<unsigned int, GroupLogData*>::const_iterator it = m_GroupData.find(groupID);
	if(it != m_GroupData.end())
	{
		it->second->HitsGiven++;
	}
}

void GameLog::RegisterAgent(Agent* agent)
{
	if(!agent)
		return;

	//m_Data.insert(pair<unsigned int, AgentLogData*>(agent->GetID(), new AgentLogData(agent->GetName(), agent->GetID(), agent->GetMask())));
	RegisterAgent(agent->GetID(), agent->GetName().c_str(), agent->GetMask());
}

void GameLog::RegisterAgent(const unsigned int agentID, const wchar_t* name, const unsigned int mask)
{
	m_Data.insert(pair<unsigned int, AgentLogData*>(agentID, new AgentLogData(name, agentID, mask)));
}

AgentLogData* GameLog::GetData(unsigned int id)
{
	std::map<unsigned int, AgentLogData*>::const_iterator it = m_Data.find(id);
	if(it != m_Data.end())
		return it->second;
	return NULL;
}

void GameLog::LogShot(unsigned int agentID)
{
	map<unsigned int, AgentLogData*>::iterator Source = m_Data.find(agentID);

	if (Source == m_Data.end()) return; 
	if (!Source->second) return; 

	Source->second->Shots += 1; 
	//LogGroupScore(Source->second->GroupID, 1);
	LogGroupShot(Source->second->GroupID);
}

void GameLog::LogDeath(unsigned int agentID)
{
	map<unsigned int, AgentLogData*>::iterator Target = m_Data.find(agentID);
	if (Target == m_Data.end()) return; 
	if (!Target->second) return; 

	Target->second->Score -= 2;
	LogGroupScore(Target->second->GroupID, -2);
	
	if (Target->second->HitsReceived.size() < 1) return;
	map<unsigned int, AgentLogData*>::iterator Source = m_Data.find(Target->second->HitsReceived.back());
	
	if (Source == m_Data.end()) return; 
	if (!Source->second) return; 

	Source->second->Score += 10;
	LogGroupScore(Source->second->GroupID, 10);

	//source killed, target died
	LogGroupDeath(Target->second->GroupID);
	Target->second->Deaths.push_back(Source->second->ID);
	
	LogGroupKill(Source->second->GroupID);
	Source->second->Kills.push_back(Target->second->ID);

}
void GameLog::LogHit(unsigned int sourceID, unsigned int targetID)
{
	map<unsigned int, AgentLogData*>::iterator Source = m_Data.find(sourceID);
	map<unsigned int, AgentLogData*>::iterator Target = m_Data.find(targetID);

	if (Source == m_Data.end() || Target == m_Data.end()) return; 
	if (!Source->second || !Target->second) return; 
	

	Target->second->HitsReceived.push_back(Source->second->ID); 
	Source->second->HitsGiven.push_back(Target->second->ID);
	LogGroupHit(Source->second->GroupID);
}

int GameLog::GetGroupScore(unsigned int groupID)
{
	std::map<unsigned int, GroupLogData*>::const_iterator it = m_GroupData.find(groupID);
	if(it != m_GroupData.end())
	{
		return it->second->Score;
	}
	return 0;
}

int GameLog::GetGroupKills(unsigned int groupID)
{
	std::map<unsigned int, GroupLogData*>::const_iterator it = m_GroupData.find(groupID);
	if(it != m_GroupData.end())
	{
		return it->second->Kills;
	}
	return 0;
}

int GameLog::GetGroupDeaths(unsigned int groupID)
{
	std::map<unsigned int, GroupLogData*>::const_iterator it = m_GroupData.find(groupID);
	if(it != m_GroupData.end())
	{
		return it->second->Deaths;
	}
	return 0;
}

float GameLog::GetGroupAccuracy(unsigned int groupID)
{
	std::map<unsigned int, GroupLogData*>::const_iterator it = m_GroupData.find(groupID);
	if(it != m_GroupData.end())
	{
		float accuracy = 0.0f;
		if(it->second->Shots > 0)
		{
			accuracy = 100.0f * static_cast<float>(it->second->HitsGiven)/static_cast<float>(it->second->Shots);
		}
		return std::min(100.0f, accuracy);
	}
	return 0.0f;
}
