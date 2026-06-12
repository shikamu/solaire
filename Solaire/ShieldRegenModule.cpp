#include "ModuleTemplates.h"

#include "ModuleTemplates.h"
#include "PropertyState.h"
#include "SpaceObject.h"
#include "LogicConstants.h"
#include "Modifier.h"
#include "AudioManager.h"

bool ShieldRegenModule::Activate(SpaceObject* target)
{
	m_Target = target; 
	return Activate();
}
bool ShieldRegenModule::Activate()
{	
	if (m_CurrentCooldown < m_Cooldown) return false;
	m_CurrentCooldown = 0.0f;

	PropertyState *Shield = m_Source->GetPropertyByID(PROP_SHIELD);
	
	if (Shield)
	{

		float Amount = (Shield->Max() - Shield->Min()) *0.05f; 

		Modifier* mShield = new Modifier();
		mShield->Lifespan(0.0f);
		mShield->ModifierValue(Amount);
		mShield->Type(POS_PERM); 
		Shield->AddModifier(mShield);

	}
	//FMODAudio::AudioManager::get().PlayFX(FMODAudio::AudioManager::get().GetClipID("data/Karl.mp3"));
	return (true);
}