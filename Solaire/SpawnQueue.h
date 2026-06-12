#pragma once
#include <queue>
#include <vector3d.h>

using std::queue;
using irr::core::vector3df;

class SpawnQueue
{
private:
	queue<vector3df> m_Group1;
	queue<vector3df> m_Group2;
	queue<vector3df> m_Group3;
	queue<vector3df> m_Group4;
	queue<vector3df> m_Default;
	void ConstructQueues();
public:
	SpawnQueue();
	~SpawnQueue();
	vector3df GetSpawn(unsigned int mask);
};