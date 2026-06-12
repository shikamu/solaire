#include "SpawnQueue.h"
#include "LogicConstants.h"
SpawnQueue::SpawnQueue()
{
	ConstructQueues();
}
SpawnQueue::~SpawnQueue()
{

}

void SpawnQueue::ConstructQueues()
{
	m_Group1.push(vector3df(-2000.0f, -1000.0f, -3000.0f));
	m_Group1.push(vector3df(1000.0f, -1000.0f, -3000.0f));
	m_Group1.push(vector3df(2000.0f, 1000.0f, -3000.0f));
	m_Group1.push(vector3df(-1000.0f, 1000.0f, -3000.0f));
	m_Group1.push(vector3df(2000.0f, -1000.0f, -3000.0f));
	m_Group1.push(vector3df(-2000.0f, 1000.0f, -3000.0f));
	m_Group1.push(vector3df(0.0f, 1000.0f, -3000.0f));
	m_Group1.push(vector3df(-1000.0f, -1000.0f, -3000.0f));
	m_Group1.push(vector3df(0.0f, -1000.0f, -3000.0f));
	m_Group1.push(vector3df(1000.0f, 1000.0f, -3000.0f));

	m_Group2.push(vector3df(-2000.0f, -1000.0f, 3000.0f));
	m_Group2.push(vector3df(1000.0f, -1000.0f, 3000.0f));
	m_Group2.push(vector3df(-2000.0f, 1000.0f, 3000.0f));
	m_Group2.push(vector3df(0.0f, 1000.0f, 3000.0f));
	m_Group2.push(vector3df(-1000.0f, -1000.0f, 3000.0f));
	m_Group2.push(vector3df(0.0f, -1000.0f, 3000.0f));
	m_Group2.push(vector3df(1000.0f, 1000.0f, 3000.0f));
	m_Group2.push(vector3df(2000.0f, 1000.0f, 3000.0f));
	m_Group2.push(vector3df(-1000.0f, 1000.0f, 3000.0f));
	m_Group2.push(vector3df(2000.0f, -1000.0f, 3000.0f));
	
	m_Group3.push(vector3df(-3000.0f, -1000.0f, -2000.0f));
	m_Group3.push(vector3df(-3000.0f, -1000.0f, 1000.0f));
	m_Group3.push(vector3df(-3000.0f, 1000.0f, -2000.0f));
	m_Group3.push(vector3df(-3000.0f, 1000.0f, 0.0f));
	m_Group3.push(vector3df(-3000.0f, -1000.0f, -1000.0f));
	m_Group3.push(vector3df(-3000.0f, -1000.0f, 0.0f));
	m_Group3.push(vector3df(-3000.0f, 1000.0f, 1000.0f));
	m_Group3.push(vector3df(-3000.0f, 1000.0f, 2000.0f));
	m_Group3.push(vector3df(-3000.0f, 1000.0f, -1000.0f));
	m_Group3.push(vector3df(-3000.0f, -1000.0f, 2000.0f));
		
	m_Group4.push(vector3df(3000.0f, -1000.0f, -2000.0f));
	m_Group4.push(vector3df(3000.0f, -1000.0f, 1000.0f));
	m_Group4.push(vector3df(3000.0f, 1000.0f, -2000.0f));
	m_Group4.push(vector3df(3000.0f, 1000.0f, 0.0f));
	m_Group4.push(vector3df(3000.0f, -1000.0f, -1000.0f));
	m_Group4.push(vector3df(3000.0f, -1000.0f, 0.0f));
	m_Group4.push(vector3df(3000.0f, 1000.0f, 1000.0f));
	m_Group4.push(vector3df(3000.0f, 1000.0f, 2000.0f));
	m_Group4.push(vector3df(3000.0f, 1000.0f, -1000.0f));
	m_Group4.push(vector3df(3000.0f, -1000.0f, 2000.0f));
}

vector3df SpawnQueue::GetSpawn(unsigned int mask)
{
	unsigned int Group = mask & GROUP_FILTER; 

	vector3df Result; 
	switch(Group)
	{
	case MASK_GROUP_1:
		Result = m_Group1.front();
		m_Group1.pop();
		m_Group1.push(Result);
		break;
	case MASK_GROUP_2:
		Result = m_Group2.front();
		m_Group2.pop();
		m_Group2.push(Result);
		break;
	case MASK_GROUP_3:
		Result = m_Group3.front();
		m_Group3.pop();
		m_Group3.push(Result);
		break;
	case MASK_GROUP_4:
		Result = m_Group4.front();
		m_Group4.pop();
		m_Group4.push(Result);
		break;
	default:
		break;
	}
	return Result;
}