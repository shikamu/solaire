#include "ModuleTemplates.h"
#include "SpaceObjectFactory.h"
#include "SpaceObject.h"
#include "LogicScene.h"
#include "GameLog.h"


bool TorpedoLauncherModule::Activate(SpaceObject* target)
{
	return Activate();
}
bool TorpedoLauncherModule::Activate()
{
	if (m_CurrentCooldown < m_Cooldown || !m_Source->GetHardTarget()) return false;
	
	GameLog::Get().LogShot(m_Source->GetAgentID());
	m_CurrentCooldown = 0.0f;

	float* values = new float[2];
	values[0] = 100.0f;
	values[1] = 150.0f;

	unsigned int ID = SpaceObjectFactory::Get().CreateWarhead(m_Source->GetParentScene(), m_Source, WARHEAD_TORPEDO, values, 2); 
	if (m_Source->GetSoftTarget())
	{
		m_Source->GetParentScene()->GetSpaceObjectByID(ID)->SetHardTarget(m_Source->GetHardTarget());
	}
	delete [] values; 
	return (ID > 0);

}