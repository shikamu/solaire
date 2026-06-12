#pragma once 
#include <irrlicht.h>

using irr::core::vector3df; 
using irr::core::stringw;

class SpaceObject;

enum MODULE_ID; 

class SpaceObjectShell
{
public:
	
	stringw Name; 
	float ModuleCooldownFraction[4];
	MODULE_ID ModuleID[4]; 
	vector3df Position, Rotation, ShieldImpactDirection; 
	
	SpaceObject* ServerSideObj;
	
	float ShieldImpactTime, ShieldRemaining, ArmourRemaining;
	unsigned int Mask, ID, AgentID, SubTypeID, TargetID;
	char TargetType;
	bool NeedsCreation, NeedsDeletion; 

	SpaceObjectShell();
	~SpaceObjectShell();
};
