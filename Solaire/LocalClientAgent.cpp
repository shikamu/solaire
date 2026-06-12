#include <vector3d.h>

#include "LocalClientAgent.h"
#include "SpaceObject.h"
#include "Actuator.h"
#include "TargettingModule.h"

using namespace irr;

LocalClientAgent::LocalClientAgent() : m_initialized(false)
{
}

LocalClientAgent::~LocalClientAgent()
{
}

void LocalClientAgent::Init()
{
	if(!m_initialized)
	{
		m_Shield = GetSpaceObject()->ShieldRemainingValue();
		m_Armour = GetSpaceObject()->ArmourRemainingValue();
		InitGUI();
		m_initialized = true;
	}
}

void LocalClientAgent::Update(float dt)
{
	if(m_initialized)
	{
		if(m_CurrentObject)
		{
			Actuator* act = m_CurrentObject->GetActuator();
			if(act)
			{
				act->update(m_CurrentObject, dt);
			}
			TargettingModule* targetting = m_CurrentObject->GetTargetter();
			if(targetting)
				targetting->UpdateTargetList(dt);
		}

		UpdateGUI(dt);
	}
}

