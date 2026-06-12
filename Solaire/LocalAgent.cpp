#include <vector3d.h>
#include <irrlicht.h>

#include "LocalAgent.h"
#include "LogicScene.h"
#include "SpaceObjectFactory.h"
#include "SpaceObject.h" 
#include "FactoryConstants.h"

using irr::scene::IMeshSceneNode;
using irr::core::vector3df;
using namespace irr; 


LocalAgent::LocalAgent(const wchar_t* name, const bool needsInit) : m_needsInitiliazation(needsInit)
{
	m_Name = name;
}

LocalAgent::~LocalAgent()
{
}

void LocalAgent::Init() 
{
	m_Score = 0; 
	if (!GetSpaceObject() && GetParentScene()) 
	{
		vector3df pos = GetParentScene()->GetNextSpawnPoint(m_Mask);
		SetSpaceObject(GetParentScene()->GetSpaceObjectByID(SpaceObjectFactory::Get().CreateShip(GetParentScene(), ACT_LOCAL, this->GetID(), this->GetName(), m_Mask, pos, pos, m_needsInitiliazation)));
		m_Shield = GetSpaceObject()->ShieldRemainingValue();
		m_Armour = GetSpaceObject()->ArmourRemainingValue();
		GetSpaceObject()->SetName(m_Name);
	}
	InitGUI(); 
}

void LocalAgent::Update(float dt) 
{
	UpdateGUI(dt); 
} 