#include "ModuleTemplates.h"
#include "PropertyState.h"
#include "SpaceObject.h"
#include "LogicConstants.h"
#include "Modifier.h"

bool DevRepairModule::Activate(SpaceObject* target)
{
	m_Target = target; 
	return Activate();
}
bool DevRepairModule::Activate()
{	
	if (m_CurrentCooldown < m_Cooldown) return false;
	m_CurrentCooldown = 0.0f;

	if (!m_Source->GetSoftTarget()) return false;

	PropertyState *Shield = m_Source->GetSoftTarget()->GetPropertyByID(PROP_SHIELD);
	PropertyState *Armour = m_Source->GetSoftTarget()->GetPropertyByID(PROP_ARMOUR);
	
	if (Shield && Armour)
	{
		Modifier* mShield = new Modifier();
		mShield->Lifespan(0.0f);
		mShield->ModifierValue(1000000.0f);
		mShield->Type(POS_PERM); 
		Shield->AddModifier(mShield);

		Modifier* mArmour = new Modifier();
		mArmour->Lifespan(0.0f);
		mArmour->ModifierValue(1000000.0f);
		mArmour->Type(POS_PERM); 
		Armour->AddModifier(mArmour);
	}

	PropertyState* Man = m_Source->GetSoftTarget()->GetPropertyByID(PROP_MANEUVERABILITY); 
	PropertyState* Thrust = m_Source->GetSoftTarget()->GetPropertyByID(PROP_THRUST); 

	if (Man)
	{
		Modifier* mMan = new Modifier();
		mMan->Lifespan(10.0f);
		mMan->ModifierValue(0.5f);
		mMan->Type(NEG_TEMP); 
		Man->AddModifier(mMan);
	}
	if (Thrust)
	{
		Modifier* mThrust = new Modifier();
		mThrust->Lifespan(10.0f);
		mThrust->ModifierValue(0.5f);
		mThrust->Type(NEG_TEMP); 
		Thrust->AddModifier(mThrust);
	}
	return (true);
}