#include "ModuleTemplates.h"
#include "SpaceObjectFactory.h"
#include "SpaceObject.h"
#include "LogicScene.h"
#include "GameLog.h"



//#ifdef _DEBUG
//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#include <crtdbg.h>
//int bulletdmgmodule = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF;
//int bulletdmgmodulef = _CrtSetDbgFlag(bulletdmgmodule); //junk variable to allow making this call
//#endif

bool RocketLauncherModule::Activate(SpaceObject* target)
{
	return Activate();
}
bool RocketLauncherModule::Activate()
{
	if (m_FireCount > 0)
	{
		if (m_SubCooldown > 0.2f)
		{
			m_FireCount--;
			m_SubCooldown = 0.0f;
			GameLog::Get().LogShot(m_Source->GetAgentID());
			return Launch();
		}
		else 
		{
			return false; 
		}
	}
	if (m_CurrentCooldown < m_Cooldown || !m_Source->GetHardTarget()) return false;
	

	m_CurrentCooldown = 0.0f;
	m_SubCooldown = 0.0f; 
	m_FireCount = 2;
	
	GameLog::Get().LogShot(m_Source->GetAgentID());
	return Launch();
}

bool RocketLauncherModule::Launch()
{
	float* values = new float[2];
	values[0] = 5.0f;
	values[1] = 30.0f;

	unsigned int ID = SpaceObjectFactory::Get().CreateWarhead(m_Source->GetParentScene(), m_Source, WARHEAD_ROCKET, values, 2); 
	if (m_Source->GetSoftTarget())
	{
		m_Source->GetParentScene()->GetSpaceObjectByID(ID)->SetHardTarget(m_Source->GetHardTarget());
	}
	delete [] values; 
	return (ID > 0);
}