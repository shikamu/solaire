#pragma once

#include <irrArray.h>
#include <IEventReceiver.h>

#include "Actuator.h"

class SpaceObject; 

class HumanActuator : public Actuator
{
public:
	HumanActuator();
	~HumanActuator();

	int enableJoystick(const bool Value, const int Number);

	void update(SpaceObject* host, const float DeltaTime);

	float getZoom() const
	{
		return m_Zoom;
	}

	bool hasChanged() const
	{
		return m_hasChanged;
	}

protected:
	ActuatorData basicUpdate(const float dt);

private:
	
	void advancedUpdate(SpaceObject* host, const float dt, const ActuatorData& basic);

	bool m_JoystickEnabled;
	irr::core::array<irr::SJoystickInfo> m_JoystickInfo;
	float m_AxisRange;
	float m_Zoom;
	
	bool m_hasChanged;
	
	float m_InRoll, m_InPitch, m_InYaw, m_InThrust, m_InZoom, m_RotationDampener; 
};

