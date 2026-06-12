#pragma once
#include "Actuator.h"
#include "CSLock.h"

class SpaceObject; 

class NetworkActuator : public Actuator
{
public:
	NetworkActuator();
	~NetworkActuator();

	virtual void update(SpaceObject* host, const float dt);

	void feed(const struct ActuatorData& data);

	Actuator* GetReplacement();

private:

	CSLock m_lock;

	ActuatorData m_data;

	NetworkActuator(const NetworkActuator& other); 
	NetworkActuator& operator=(const NetworkActuator& other); 
};
