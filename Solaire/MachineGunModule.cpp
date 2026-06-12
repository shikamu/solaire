#include "ModuleTemplates.h"
#include "SpaceObjectFactory.h"
#include "SpaceObject.h"
#include "GameLog.h"
//#ifdef _DEBUG
//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#include <crtdbg.h>
//int bulletdmgmodule = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF;
//int bulletdmgmodulef = _CrtSetDbgFlag(bulletdmgmodule); //junk variable to allow making this call
//#endif

bool MachineGunModule::Activate(SpaceObject* target)
{
	return Activate();
}
bool MachineGunModule::Activate()
{
	if (m_CurrentCooldown < m_Cooldown) return false;
	GameLog::Get().LogShot(m_Source->GetAgentID());

	m_CurrentCooldown = 0.0f;

	float* values = new float[2];
	values[0] = 5.0f;
	values[1] = 1.0f;

	unsigned int ID = SpaceObjectFactory::Get().CreateProjectile(m_Source->GetParentScene(), m_Source, PROJECTILE_EMP, values, 2); 


	delete [] values; 
	return (ID > 0);

}