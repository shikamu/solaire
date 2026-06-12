#pragma once

#include <vector3d.h>

struct SpaceObjectNetworkInfo
{
	SpaceObjectNetworkInfo();

	unsigned int id;//SpaceObject's ID
	unsigned int targetID;
	float impactTime; 
	float shieldRemaining;
	float ArmourRemaining; 
	char targetType; 
	irr::core::vector3df hitDirection;
	irr::core::vector3df position;
	irr::core::vector3df rotation;
};

