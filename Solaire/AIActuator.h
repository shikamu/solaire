#pragma once
#include "Actuator.h"

class SpaceObject; 
class AIController; 

class AIActuator : public Actuator
{
private:
	AIController* m_AI; 
	bool m_AIInitialised;
public:
	AIActuator();
	AIActuator(irr::core::quaternion& current); 
	~AIActuator();

	void update(SpaceObject* host, const float dt);
};