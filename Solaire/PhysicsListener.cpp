#include "PhysicsListener.h"
#include "PhysicsObject.h"
#include "SpaceObject.h"
#include "RenderObjectIncludes.h"

void PhysicsListener::AddTarget(PhysicsObject* obj)
{
	bool TargetExists = false;
	for (auto i = m_Targets.begin(); i < m_Targets.end(); i++)
	{
		if (obj->m_Parameters.ID == (*i)->ID) TargetExists = true; 
	}
	if (TargetExists) return; 
	m_Targets.push_back(obj->m_Parameters.Listener->GetHost());
}

void PhysicsListener::CleanTargets()
{
	for (auto i = m_Targets.begin(); i != m_Targets.end();)
	{
		if ((m_Host->GetRenderObject()->GetPosition() - (*i)->GetRenderObject()->GetPosition()).getLength() > m_SensorRange)
		{
			i = m_Targets.erase(i);
		}
		else i++;
	}
}