#include <map>
#include <IMeshSceneNode.h>
#include <irrList.h>

#include "TargettingModule.h"
#include "SpaceObject.h"
#include "LogicScene.h"
#include "LogicConstants.h" 
#include "RenderObjectIncludes.h"

using std::map;
	
TargettingModule::TargettingModule(SpaceObject* host, LogicScene *scene) : m_Host(host), m_ParentScene(scene), m_ActivationTimer(0.0f), m_HardLockTimer (0.0f), m_hardLockTimerFraction(0.0f), m_previousTarget(0), m_currentTarget(0)
{
	m_Range = host->GetPropertyByID(PROP_SENSOR_RANGE)->CurrentPointer();

	m_Speed = host->GetPropertyByID(PROP_SENSOR_SPEED)->CurrentPointer();

	m_Accuracy = host->GetPropertyByID(PROP_SENSOR_ACCURACY)->CurrentPointer();
}

void TargettingModule::UpdateTargetList(float dt)
{
	if (!m_ParentScene) return; 
	if (!m_Range) return; 

	for (auto i = m_Targets.begin(); i < m_Targets.end();)
	{
		if ((m_Host->GetRenderObject()->GetPosition() - (*i)->GetRenderObject()->GetPosition()).getLength() > *m_Range)
		{
			if (m_Host->GetHardTarget())
			{
				if ((*i)->ID == m_Host->GetHardTarget()->ID) m_Host->SetHardTarget(NULL);
			}
			if (m_Host->GetSoftTarget())
			{
				if ((*i)->ID == m_Host->GetSoftTarget()->ID && m_Host->GetSoftTarget()) m_Host->SetSoftTarget(NULL);
			}
			i = m_Targets.erase(i);
		}
		else ++i;
	}
	
	const map<unsigned int, SpaceObject*> ObjectList = m_ParentScene->GetObjectList();
	for (auto i = ObjectList.begin(); i != ObjectList.end(); i++)
	{
		if ((*i).first == m_Host->ID || ((*i).second->ObjectMask & (MASK_SHIP | MASK_STATIC)) == 0) continue;
		if ((m_Host->GetRenderObject()->GetPosition() - (*i).second->GetRenderObject()->GetPosition()).getLength() > *m_Range) continue;

		bool NewTarget = true; 
		for (auto j = m_Targets.begin(); j < m_Targets.end(); j++)
		{
			if ((*i).first == (*j)->ID) NewTarget = false; 
		}
		if (!NewTarget) continue; 

		m_Targets.push_back((*i).second);
	}
	m_ActivationTimer += dt;

	//float HardTimer = 10.0f; 
	m_hardTimer = 10.0f;
	if (m_Speed)
	{
		//HardTimer = *m_Speed; 
		m_hardTimer = *m_Speed;
	}
	float HardThreshold = 0.98f;
	if (m_Accuracy)
	{
		HardThreshold = 1.0f - *m_Accuracy;
	}

	SpaceObject* soft = m_Host->GetSoftTarget();
	if (soft)
	{
		/*
		SpaceObject* prev = m_ParentScene->GetSpaceObjectByID(m_previousTarget);
		if(prev)
		{
			RenderObject* prevRobj = prev->GetRenderObject();
			irr::scene::IMeshSceneNode* prevOutlineNode = NULL;
			if(prevRobj && (prevOutlineNode = prevRobj->getOutlineNode()) && prevOutlineNode->isVisible())
			{
				irr::u32 alpha = static_cast<irr::u32>(irr::core::lerp((irr::u32)255, (irr::u32)0, m_hardLockTimerFraction));
				prevRobj->setOutlineShaderTime(alpha/255.0f);
				prevOutlineNode->setMaterialType((irr::video::E_MATERIAL_TYPE)prevRobj->getOutlineShaderMaterial());
				if(alpha < 5)
					prevOutlineNode->setVisible(false);
			}
		}

		RenderObject* robj = soft->GetRenderObject();
		irr::scene::IMeshSceneNode* outlineNode = NULL;
		if(robj && (outlineNode = robj->getOutlineNode()))
		{
			m_hardLockTimerFraction = m_HardLockTimer/HardTimer;
			outlineNode->setVisible(true);
			outlineNode->setMaterialType(irr::video::EMT_SOLID);
			if(m_Host->GetHardTarget())
			{
				m_hardLockTimerFraction = 1.0f;
				outlineNode->getMaterial(0).EmissiveColor.set(255, 0, 255, 0);
			}
			else
			{
				irr::u32 r = irr::core::lerp(255, 0, std::min(1.0f, m_hardLockTimerFraction));
				irr::u32 g = irr::core::lerp(0, 255, std::min(1.0f, m_hardLockTimerFraction));
				outlineNode->getMaterial(0).EmissiveColor.set(255, r, g, 0);
			}
		}
		*/
		
		vector3df direction = m_Host->GetRenderObject()->GetRotation().rotationToDirection(vector3df(0.0f, 0.0f, 1.0f));
		vector3df targetPosition = soft->GetRenderObject()->GetPosition() - m_Host->GetRenderObject()->GetPosition();
		direction.normalize();
		targetPosition.normalize();

		if (direction.dotProduct(targetPosition) > HardThreshold)
		{
			m_HardLockTimer += dt;
		}

		if (m_HardLockTimer > m_hardTimer)
		{
			m_Host->SetHardTarget(soft);
			m_HardLockTimer = 0.0f;
		}
	}
	else m_HardLockTimer = 0.0f;
}

void TargettingModule::NextSoftTarget()
{
	if (m_ActivationTimer < 0.2f) return;
	m_ActivationTimer = 0.0f;
	m_HardLockTimer = 0.0f;
	m_hardLockTimerFraction = 0.0f;

	if (m_Targets.size() == 0 || !m_Range) return; 

	if (!m_Host->GetSoftTarget())
	{
		SpaceObject* obj = *m_Targets.begin();
		m_Host->SetSoftTarget(obj);
		m_currentTarget = obj->ID;
		return;
	}
	else if (m_Targets.size() > 1)
	{
		bool NewTarget = false; 
		for (auto i = m_Targets.begin(); i < m_Targets.end(); i++)
		{
			if (NewTarget) break;
			if ((*i)->ID == m_Host->GetSoftTarget()->ID)
			{
				if ((i + 1) != m_Targets.end()) m_Host->SetSoftTarget(*(i+1));
				else m_Host->SetSoftTarget(*m_Targets.begin());
				NewTarget = true; 
			}
		}
		if(NewTarget)
		{
			if(m_currentTarget)
				m_previousTarget = m_currentTarget;

			m_currentTarget = m_Host->GetSoftTarget()->ID;
		}
	}
	if (m_Host->GetHardTarget())
	{
		m_Host->SetHardTarget(NULL);
		m_HardLockTimer = 0.0f;
	}
}

void TargettingModule::PrevSoftTarget()
{
	if (m_ActivationTimer < 0.2f) return;
	m_ActivationTimer = 0.0f;

	m_HardLockTimer = 0.0f;
	m_hardLockTimerFraction = 0.0f;

	if (m_Targets.size() == 0 || !m_Range) return; 

	if (!m_Host->GetSoftTarget())
	{
		SpaceObject* obj = m_Targets.back();
		m_Host->SetSoftTarget(obj);
		//std::cout << "(1) m_currentTarget=" << m_currentTarget << ", going to change it to " << obj->ID << std::endl;
		m_currentTarget = obj->ID;
		return; 
	}
	else if (m_Targets.size() > 1)
	{
		bool NewTarget = false;
		for (auto i = m_Targets.begin(); i < m_Targets.end(); i++)
		{
			if (NewTarget) break;
			if ((*i)->ID == m_Host->GetSoftTarget()->ID)
			{
				if ( i != m_Targets.begin()) m_Host->SetSoftTarget(*(i-1));
				else m_Host->SetSoftTarget(m_Targets.back());
				NewTarget = true; 
			}
		}
		if(NewTarget)
		{
			if(m_currentTarget)
				m_previousTarget = m_currentTarget;

			//std::cout << "(2) m_currentTarget=" << m_currentTarget << ", going to change it to " << m_Host->GetSoftTarget()->ID << "; m_previousTarget=" << m_previousTarget << std::endl;
			m_currentTarget = m_Host->GetSoftTarget()->ID;
		}
	}
	if (m_Host->GetHardTarget())
	{
		m_Host->SetHardTarget(NULL);
		m_HardLockTimer = 0.0f;
	}
}

void TargettingModule::FrontSoftTarget()
{
	if (m_ActivationTimer < 0.2f) return;
	m_ActivationTimer = 0.0f;

	m_HardLockTimer = 0.0f;
	m_hardLockTimerFraction = 0.0f;

	if (m_Targets.size() == 0 || !m_Range) return; 
	SpaceObject* frontTarget = NULL; 


	vector3df forwardDirection = m_Host->GetRenderObject()->GetSceneNode()->getRotation().rotationToDirection(vector3df(0.0f, 0.0f, 1.0f));
	float highestValue = 0.0f; 


	for (auto i = m_Targets.begin(); i <  m_Targets.end(); i++)
	{
		float DP = forwardDirection.dotProduct(((*i)->GetRenderObject()->GetPosition() - m_Host->GetRenderObject()->GetPosition()).normalize());
		if (DP > 0.7f && DP > highestValue)
		{
			frontTarget = (*i);
			highestValue = DP;
		}
	}

	if (!frontTarget) return; 
	bool NewTarget = false; 
	if (!m_Host->GetSoftTarget() && !m_Host->GetHardTarget())
	{
		m_Host->SetSoftTarget(frontTarget);
		NewTarget = true; 
	}
	else if (!m_Host->GetHardTarget())
	{
		if (m_Host->GetSoftTarget()->ID != frontTarget->ID)
		{
			m_Host->SetSoftTarget(frontTarget);
			NewTarget = true; 
		}
	}
	else
	{
		if (m_Host->GetHardTarget()->ID != frontTarget->ID)
		{
			m_Host->SetSoftTarget(frontTarget);
			m_Host->SetHardTarget(NULL);
			NewTarget = false; 
		}
	}
	if(NewTarget)
	{
		if(m_currentTarget)
			m_previousTarget = m_currentTarget;

		//std::cout << "(2) m_currentTarget=" << m_currentTarget << ", going to change it to " << m_Host->GetSoftTarget()->ID << "; m_previousTarget=" << m_previousTarget << std::endl;
		m_currentTarget = m_Host->GetSoftTarget()->ID;
	}
}