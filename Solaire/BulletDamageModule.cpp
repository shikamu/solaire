#include "ModuleTemplates.h"
#include "SpaceObject.h"
#include "PropertyState.h"
#include "Modifier.h"
#include "LogicConstants.h"
#include "GameLog.h"


bool BulletDamageModule::Activate()
{
	PropertyState *Shield = m_Target->GetPropertyByID(PROP_SHIELD);
	PropertyState *Armour = m_Target->GetPropertyByID(PROP_ARMOUR);
	PropertyState *Integrity = m_Target->GetPropertyByID(PROP_SHIELD_INTEGRITY);

	GameLog::Get().LogHit(m_Source->GetAgentID(), m_Target->GetAgentID());

	if (Shield && Armour && Integrity)
	{
		float ShieldFrac = Integrity->Current();

		if (Shield->Current() >= m_ShieldDmg)
		{

			Modifier* mShield = new Modifier();
			mShield->Lifespan(0.0f);
			mShield->ModifierValue(m_ShieldDmg * ShieldFrac);
			mShield->Type(NEG_PERM); 
			Shield->AddModifier(mShield);

			Modifier* mArmour = new Modifier();
			mArmour->Lifespan(0.0f);
			mArmour->ModifierValue(m_ArmourDmg * (1.0f - ShieldFrac));
			mArmour->Type(NEG_PERM); 
			Armour->AddModifier(mArmour);

			return true;
		}
		else if (Shield->Current() > 0.0)
		{
			float actualShieldFrac = Shield->Current() / m_ShieldDmg; 


			Modifier* mShield = new Modifier();
			mShield->Lifespan(0.0f);
			mShield->ModifierValue(Shield->Current());
			mShield->Type(NEG_PERM); 
			Shield->AddModifier(mShield);

			Modifier* mArmour = new Modifier();
			mArmour->Lifespan(0.0f);
			mArmour->ModifierValue(m_ArmourDmg * (1.0f - actualShieldFrac));
			mArmour->Type(NEG_PERM); 
			Armour->AddModifier(mArmour);

			return true;
		}
		else 
		{
			Modifier* m = new Modifier();
			m->Lifespan(0.0f);
			m->ModifierValue(m_ArmourDmg);
			m->Type(NEG_PERM); 
			Armour->AddModifier(m);
			return true;
		}
	}
	else if (Shield)
	{
		if (Shield->Current() >= m_ShieldDmg)
		{
			Modifier* m = new Modifier();
			m->Lifespan(0.0f);
			m->ModifierValue(m_ShieldDmg);
			m->Type(NEG_PERM); 
			Shield->AddModifier(m);
		}
		return true;
	}
	else if (Armour)
	{
		return true;
	}
	else return false; 
}

bool BulletDamageModule::Activate(SpaceObject* target)
{
	m_Target = target;
	return Activate();
}
