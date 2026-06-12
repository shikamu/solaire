#pragma once
#include "Actuator.h"

class SpaceObject; 
class AIController; 

class AIActuator : public Actuator
{
private:
	AIController* m_AI;
	bool m_AIInitialised;
	bool m_advanced;
public:
	AIActuator(const bool advanced = false);
	AIActuator(irr::core::quaternion& current, const bool advanced = false);
	~AIActuator();

	void update(SpaceObject* host, const float dt);
};