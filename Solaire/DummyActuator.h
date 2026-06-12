#pragma once
#include "Actuator.h"

class SpaceObject; 

class DummyActuator : public Actuator
{
private:
public:
	DummyActuator();
	DummyActuator(irr::core::quaternion& current); 
	~DummyActuator();

	void update(SpaceObject* host, const float dt);
};