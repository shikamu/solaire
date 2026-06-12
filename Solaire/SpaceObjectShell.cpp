#include "SpaceObjectShell.h"
#include "LogicConstants.h"
#include "FactoryConstants.h"

SpaceObjectShell::SpaceObjectShell() : Name (L"None"), ServerSideObj(NULL), ShieldImpactTime (0.0f), ShieldRemaining(1.0f), ArmourRemaining(1.0f), 
	Mask(MASK_RESERVED), ID(0), AgentID(0), SubTypeID(0), TargetID(0), TargetType(TARGET_NONE), NeedsCreation(true), NeedsDeletion(false)   
{
	for (int i = 0; i < 4; i++)
	{
		ModuleCooldownFraction[i] = 1.0f;
		ModuleID[i] = MOD_NONE; 
	}
}

SpaceObjectShell::~SpaceObjectShell()
{
}