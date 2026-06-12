#pragma once
#include <vector3d.h>
#include <irrTypes.h>

class PhysicsListener; 

class PhysicsParams
{
public:
	float Mass, MassInverse, LinearThrust, Size, Drag, Lifespan, MaxVelocity;
	irr::u32 ID, ParentID; // ID is ID of spaceobject
	unsigned int ObjectMask; // Object type/group mask
	bool IsVolatile, HasLifespan; 
	PhysicsListener* Listener; 
	irr::core::vector3df InitialPosition, InitialVelocity;
	void SetInverseMass()
	{
		MassInverse = 1.0f / Mass;
	}
	PhysicsParams() : Mass (1.0f), MassInverse(1.0f), LinearThrust (1.0f), Size (1.0f), Drag (0.0f), Lifespan (-1.0f), 
		ID (0), ParentID(0), IsVolatile (false), HasLifespan (false), Listener (NULL), MaxVelocity (1000.0f), ObjectMask (0) {}



};
