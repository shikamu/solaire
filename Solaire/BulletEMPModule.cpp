#include "ModuleTemplates.h"
#include "SpaceObject.h"
#include "PropertyState.h"
#include "Modifier.h"
#include "LogicConstants.h"
#include "GameLog.h"


bool BulletEMPModule::Activate()
{
	PropertyState *Thrust = m_Target->GetPropertyByID(PROP_THRUST);
	PropertyState *Man = m_Target->GetPropertyByID(PROP_MANEUVERABILITY);

	GameLog::Get().LogHit(m_Source->GetAgentID(), m_Target->GetAgentID());

	bool EffectApplied = false;
	if (Man)
	{
			Modifier* mMan = new Modifier();
			mMan->Lifespan(5.0f);
			mMan->ModifierValue(m_ManMod);
			mMan->Type(NEG_TEMP); 
			Man->AddModifier(mMan);
			EffectApplied = true;
	}
	if (Thrust)
	{
			Modifier* mThrust = new Modifier();
			mThrust->Lifespan(5.0f);
			mThrust->ModifierValue(m_ManMod);
			mThrust->Type(NEG_TEMP); 
			Thrust->AddModifier(mThrust);
			EffectApplied = true;
	}
	return EffectApplied;
}

bool BulletEMPModule::Activate(SpaceObject* target)
{
	m_Target = target;
	return Activate();
}