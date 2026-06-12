#include <irrlicht.h>
#include <iostream>
#include "SpaceObject.h"
#include "Actuator.h"
#include "PhysicsObject.h"
#include "RenderObjectIncludes.h"
#include "LogicScene.h"
#include "TargettingModule.h"
#include "LockPointer.h"
#include "LogicConstants.h"
#include "LogicModule.h"
#include "SpaceObjectShell.h"
#include "GameLog.h"
#include "System.h"
#include "NetworkController.h"
#include "LANServer.h"

using std::pair;

SpaceObject::~SpaceObject()
{
	delete m_Targetter; 
	delete m_RenderObj; 
	delete m_Actuator; 
	//for(map<PropertyTypeID, PropertyState>::const_iterator it = m_Properties.begin(); it != m_Properties.end();)
	//{
	//	it = m_Properties.erase(it);
	//}
	for (auto i = Modules.begin(); i < Modules.end(); i++)
	{
		delete (*i);
	}
}

void SpaceObject::AddPropertyState(const PropertyTypeID type, const PropertyState& state)
{
	pair<PropertyTypeID, PropertyState> newProperty; 
	newProperty.first = type;
	newProperty.second = state;

	m_Properties.insert(newProperty);
}

PropertyState* SpaceObject::GetPropertyByID(PropertyTypeID ID)
{
	map<PropertyTypeID, PropertyState>::iterator it = m_Properties.find(ID);
	if (it != m_Properties.end())
	{
		return &(it->second);
	}
	else return NULL; 
}

void SpaceObject::Update(float dt)
{
	if (NeedsDeletion())
	{
		if (m_Shell)
		{
			SpaceObjectShell* shell = m_Shell->GetPointer();
			if (shell)
			{
				//shell->ServerSideObj = NULL; 
				shell->NeedsDeletion = true; 
			}
			m_Shell->Unlock();
			m_Shell = NULL;
		}
		m_RenderObj->GetSceneNode()->setVisible(false); 
	}
	if(!NeedsDeletion())
	{
		if (m_RenderObj) m_RenderObj->Update(dt);
		CleanTargets();
		if (m_Actuator)
		{
			if(m_Actuator->NeedsReplacement())
			{
				Actuator* act = m_Actuator->GetReplacement();
				delete m_Actuator;
				m_Actuator = act;
			}
			m_Actuator->update(this, dt);
			if (m_RenderObj) m_RenderObj->SetRotation(m_Actuator->getRotation());
		}

		//if (NeedsDeletion()) return;
		PhysicsObject* obj = m_PhysicsObj->GetPointer();
		if(obj)
		{
			if(m_Actuator)
			{
				obj->ApplyThrust(m_Actuator->GetDirection(), m_Actuator->GetThrust());
			}
			if (m_RenderObj) m_RenderObj->SetPosition(obj->GetPosition());
			m_Velocity = obj->GetVelocity(); 

		}
		m_PhysicsObj->Unlock();

		for (auto i = Modules.begin(); i < Modules.end(); i++)
		{
			if (!(*i)->FittedModule) continue;
			(*i)->FittedModule->Update(dt);

			if (!m_Actuator) continue;
			if (m_Actuator->CheckActType((*i)->ActivationType)) (*i)->FittedModule->Activate();
		}

		for(map<PropertyTypeID, PropertyState>::iterator it =  m_Properties.begin(); it != m_Properties.end(); ++it)
		{
			it->second.ApplyModifiers(dt);
		}

		if (m_Targetter) m_Targetter->UpdateTargetList(dt);
		if (m_Actuator && m_Targetter)
		{
			if (m_Actuator->CheckButton(INPUT_FRONT))
			{
				m_Targetter->FrontSoftTarget();
			}
			else if (m_Actuator->CheckButton(INPUT_NEXT))
			{
				m_Targetter->NextSoftTarget();
			}
			else if (m_Actuator->CheckButton(INPUT_PREV))
			{
				m_Targetter->PrevSoftTarget();
			}
		}


		if (m_ShieldImpactTime > 0.0f)
		{
			m_ShieldImpactTime -= dt; 
		}
		else m_ShieldImpactTime = 0.0f;	

		PropertyState *Shield = GetPropertyByID(PROP_SHIELD);
		if (Shield)
		{
			m_ShieldRemaining = *Shield->CurrentFractionPointer();
		}

		PropertyState *Armour = GetPropertyByID(PROP_ARMOUR);
		if (Armour)
		{
			m_ArmourRemaining = *Armour->CurrentFractionPointer(); 
		}
		DeathCheck(); 

		if (m_Shell)
		{
			SpaceObjectShell* shell = m_Shell->GetPointer();
			if (shell && m_RenderObj)
			{
				shell->Position = m_RenderObj->GetPosition();
				shell->Rotation = m_RenderObj->GetRotation();
				if (GetPropertyByID(PROP_SHIELD))
				{
					shell->ShieldImpactDirection = m_ShieldImpactDirection;
					shell->ShieldImpactTime = m_ShieldImpactTime;
					shell->ShieldRemaining = m_ShieldRemaining;
				}
				PropertyState* armour = GetPropertyByID(PROP_ARMOUR);
				if (armour)
				{
					shell->ArmourRemaining = *armour->CurrentFractionPointer();
				}
				if (m_Targetter)
				{
					if (GetHardTarget())
					{
						shell->TargetID = GetHardTarget()->ID;
						shell->TargetType = TARGET_HARD;
					}
					else if (GetSoftTarget())
					{
						shell->TargetID = GetSoftTarget()->ID; 
						shell->TargetType = TARGET_SOFT;
					}
					else 
					{
						shell->TargetID = 0; 
						shell->TargetType = TARGET_NONE;
					}
				}
			}
			m_Shell->Unlock();
		}
	}
}

void SpaceObject::SetNetworkTarget(const vector3df& pos, const vector3df& rot)
{
	m_NetTargetPosition = pos;
	m_NetTargetRotation = rot;
	if(!m_HasNetTarget)
	{
		//First update for this object: snap straight to it so it doesn't visibly slide in
		//from wherever it was spawned/created.
		m_HasNetTarget = true;
		if(m_RenderObj)
		{
			m_RenderObj->SetPosition(pos);
			m_RenderObj->SetRotation(rot);
		}
	}
}

void SpaceObject::InterpolateToNetworkTarget(float dt)
{
	if(!m_HasNetTarget || !m_RenderObj)
		return;

	//Exponential smoothing toward the latest server transform. factor ~= fraction of the
	//remaining gap closed this frame; clamped so a long frame can't overshoot.
	float factor = dt * 12.0f;
	if(factor > 1.0f) factor = 1.0f;
	if(factor < 0.0f) factor = 0.0f;

	//Position: straight lerp.
	vector3df pos = m_RenderObj->GetPosition();
	pos += (m_NetTargetPosition - pos) * factor;
	m_RenderObj->SetPosition(pos);

	//Rotation: slerp via quaternions so we take the shortest arc and avoid Euler wraparound
	//artifacts (e.g. snapping the "long way" around from 359 to 1 degrees).
	vector3df curRot = m_RenderObj->GetRotation();
	irr::core::quaternion qCurrent(curRot.X * irr::core::DEGTORAD, curRot.Y * irr::core::DEGTORAD, curRot.Z * irr::core::DEGTORAD);
	irr::core::quaternion qTarget(m_NetTargetRotation.X * irr::core::DEGTORAD, m_NetTargetRotation.Y * irr::core::DEGTORAD, m_NetTargetRotation.Z * irr::core::DEGTORAD);
	irr::core::quaternion qResult;
	qResult.slerp(qCurrent, qTarget, factor);
	vector3df newRot;
	qResult.toEuler(newRot);
	newRot *= irr::core::RADTODEG;
	m_RenderObj->SetRotation(newRot);
}

void SpaceObject::OnDestroy()
{
	FlagForDeletion();
}

void SpaceObject::OnCollide(const unsigned int OtherID, const unsigned int OtherParentID) 
{
	SpaceObject* target = m_ParentScene->GetSpaceObjectByID(OtherID);
	if(target)
	{
		m_ShieldImpactDirection = target->GetRenderObject()->GetPosition() - GetRenderObject()->GetPosition();
		m_ShieldImpactDirection.normalize();
		m_ShieldImpactTime = 1.0f;
		if ((m_Host->ObjectMask & MASK_SHIP) != 0) return; 
		// NEED TWO CSLOCKS - 
		//		First one locks the module list, to prevent access whilst the list is being created/destroyed
		//		Second is in the property state, to avoid modifiers being added to the list whilst
		//		it is already being applied, or whilst modifiers are being destroyed 

		m_ModuleListLock.Lock();
		for (auto i = Modules.begin(); i < Modules.end(); i++)
		{
			if (!(*i)->FittedModule) continue;
			if ((*i)->ActivationType == ACT_COLLISION)
			{
				(*i)->FittedModule->Activate(target);
			}
		}
		m_ModuleListLock.Unlock();
	}
} 

void SpaceObject::DeathCheck()
{
	PropertyState* Shield = GetPropertyByID(PROP_SHIELD);
	PropertyState* Armour = GetPropertyByID(PROP_ARMOUR);

	if (Armour)
	{
		if (Armour->Current() < 0.001f)
		{
			Die();
		}
	}
	else if (Shield)
	{
		if (Shield->Current() < 0.001f)
		{
			Die(); 
		}
	}

}

void SpaceObject::Die()
{
	for (auto i = m_Properties.begin(); i != m_Properties.end(); i++)
	{
		(*i).second.Reset();
	}		
	float Size = 100.0f; 
	PhysicsObject* obj = m_PhysicsObj->GetPointer();
	if(obj)
	{
		obj->Reset(m_ParentScene->GetNextSpawnPoint(ObjectMask));
	}
	m_PhysicsObj->Unlock();

	unsigned int killerID = GameLog::Get().LogDeath(m_AgentID);

	//Kill feed: build "<killer> destroyed <victim>" (or "<victim> was destroyed" for a suicide),
	//show it locally, and in a networked game broadcast it to the clients' overlays.
	{
		AgentLogData* victimData = GameLog::Get().GetData(m_AgentID);
		stringw victimName = victimData ? victimData->Name : GetName();
		stringw killMsg;
		if(killerID != 0 && killerID != m_AgentID)
		{
			AgentLogData* killerData = GameLog::Get().GetData(killerID);
			stringw killerName = killerData ? killerData->Name : stringw(L"Someone");
			killMsg = killerName;
			killMsg += L" destroyed ";
			killMsg += victimName;
		}
		else
		{
			killMsg = victimName;
			killMsg += L" was destroyed";
		}

		System::get().pushGameNotification(killMsg.c_str());

		LANServer* server = m_ParentScene->IsNetworked() ? NetworkController::get().getServer() : NULL;
		if(server)
			server->broadcastKillFeed(killMsg.c_str());

		//End-of-match: first team to the kill limit wins; announce it once.
		unsigned int winnerGroup = GameLog::Get().CheckForWinner();
		if(winnerGroup != 0)
		{
			const int teamNum = (winnerGroup == MASK_GROUP_1) ? 1 : (winnerGroup == MASK_GROUP_2) ? 2 : (winnerGroup == MASK_GROUP_3) ? 3 : 4;
			stringw banner(L"*** TEAM ");
			banner += teamNum;
			banner += L" WINS! ***";
			System::get().pushGameNotification(banner.c_str());
			if(server)
				server->broadcastKillFeed(banner.c_str());
		}
	}

	SetSoftTarget(NULL);
	SetHardTarget(NULL);
	vector3df ExpPos = GetRenderObject()->GetPosition();
	vector3df ExpRot = GetRenderObject()->GetRotation();
	m_ParentScene->CreateExplosion(ExpPos, ExpRot, Size, ObjectMask);
	if(m_ParentScene->IsNetworked())
	{
		//TODO send message to tell clients to call CreateExplosion(ExpPos, ExpRot, Size, ObjectMask);
	}
}