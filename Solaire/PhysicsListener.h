#pragma once
#include <vector3d.h>
#include <vector>
//#include "LockPointer.h"

using irr::core::vector3df;
using std::vector;
class PhysicsObject;
class SpaceObject; 

class PhysicsListener
{
protected:
	vector<SpaceObject *> m_Targets;
	SpaceObject* m_Host; 
	float m_SensorRange; 
public:

	PhysicsListener(SpaceObject* host) : m_Host (host), m_SensorRange (0.0f) {}
	PhysicsListener() : m_Host (NULL), m_SensorRange (0.0f) {}
	virtual ~PhysicsListener() {} 
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnCollide(const unsigned int OtherID, const unsigned int OtherParentID) {} 
	virtual void Update(vector3df& position, vector3df& velocity) {}
	void CleanTargets();
	void AddTarget(PhysicsObject* obj);
	void PLSensorRange(float range)
	{
		m_SensorRange = range; 
	}
	float PLSensorRange() 
	{
		return m_SensorRange; 
	}

	SpaceObject* GetHost()
	{
		return m_Host;
	}

};
