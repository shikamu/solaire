#include <vector3d.h>

#include "DummyAgent.h" 
#include "AIActuator.h"
#include "LogicScene.h"
#include "SpaceObjectFactory.h"
#include "SpaceObject.h" 

using irr::core::vector3df;

void DummyAgent::Init()
{
	m_Score = 0; 
	if (!GetSpaceObject() && GetParentScene()) 
	{
		vector3df pos = GetParentScene()->GetNextSpawnPoint(m_Mask);
		SetSpaceObject(GetParentScene()->GetSpaceObjectByID(SpaceObjectFactory::Get().CreateShip(GetParentScene(), ACT_AI_BASIC, this->GetID(), this->GetName(), m_Mask, pos, pos)));
		GetSpaceObject()->SetName(m_Name);
	}
}
void DummyAgent::Update(float dt)
{

}

