#pragma once
#include "Actuator.h"
class SpaceObject; 

class WarheadActuator : public Actuator
{
private:
	float m_Fuse;
public:
	WarheadActuator() : m_Fuse(0.0f) {} 
	WarheadActuator(float fuse) : m_Fuse(fuse) {} 
	~WarheadActuator() {}

	void update(SpaceObject* host, const float dt);
};