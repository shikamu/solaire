#pragma once 
#include <vector>
#include "SceneNodeParams.h"
#include "FactoryConstants.h"
#include <vector3d.h>
#include <irrString.h> 

class Agent; 
class LogicScene; 
class SpaceObject; 
class SpaceObjectShell; 
enum ACTUATOR_TYPE;

using irr::core::vector3df;
using irr::core::stringw;

class SpaceObjectFactory
{
private:
	static SpaceObjectFactory m_Factory; 
public:
	static SpaceObjectFactory& Get();
	SpaceObjectFactory() {}
	~SpaceObjectFactory() {}

	unsigned int CreateProjectile(LogicScene* parentScene, SpaceObject* hostObj, const PROJECTILE_TYPE type, float* values, const int valueCount);
	unsigned int CreateShip(LogicScene* parentScene, ACTUATOR_TYPE actType, unsigned int agentID, stringw name, const unsigned int mask, vector3df& pos, vector3df& rot, const bool needsCreation = true); 
	unsigned int CreateWarhead(LogicScene* parentScene, SpaceObject* hostObj, const WARHEAD_TYPE type, float* values, const int valueCount);
	unsigned int CreateObjectFromShell(LogicScene* parentScene, SpaceObjectShell* shell); 
};