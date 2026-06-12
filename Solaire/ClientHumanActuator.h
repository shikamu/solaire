#pragma once
#include "HumanActuator.h"

class ClientHumanActuator : public HumanActuator
{
public:
	ClientHumanActuator(const unsigned int id);
	~ClientHumanActuator();

	void update(SpaceObject* host, const float dt);

	const ActuatorOutput& getCurrent() const;

private:
	ActuatorOutput m_currentOutput;

	unsigned int m_id;

};

