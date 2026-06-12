#include <irrlicht.h>

#include "NetworkActuator.h"
#include "SpaceObject.h"
#include "PropertyState.h"
#include "LogicConstants.h"
#include "DummyActuator.h"

using namespace irr;

NetworkActuator::NetworkActuator() 
{

}

NetworkActuator::~NetworkActuator()
{

}

void NetworkActuator::update(SpaceObject* host, const float dt)
{
	m_lock.Lock();

	float RotationFactor = 1.0f;
	if (host)
	{
		PropertyState* HostMan = host->GetPropertyByID(PROP_MANEUVERABILITY);
		PropertyState* HostThrust = host->GetPropertyByID(PROP_THRUST);
		if (HostMan)
		{
			RotationFactor = HostMan->Current() ; 
		}
		if (HostThrust)
		{
			m_Thrust *= HostThrust->Current();
		}
	}

	m_FRoll = -m_data.RollVal * (1.0f - m_data.ThrustVal * 0.8f);
	m_FPitch = -m_data.PitchVal * (1.0f - m_data.ThrustVal * 0.8f); 
	m_FYaw = m_data.YawVal * (1.0f - m_data.ThrustVal * 0.8f); 
	m_Thrust = m_data.ThrustVal; 

	m_QRoll.fromAngleAxis(m_FRoll * RotationFactor * dt, core::vector3df(0, 0, 1)); 
	m_QRoll.normalize();

	m_QPitch.fromAngleAxis(m_FPitch * RotationFactor * dt, core::vector3df(1, 0, 0));
	m_QPitch.normalize();

	m_QYaw.fromAngleAxis(m_FYaw * RotationFactor * dt, core::vector3df(0, 1, 0));
	m_QYaw.normalize();

	core::quaternion combination = m_QRoll*m_QPitch*m_QYaw;
	combination.normalize();
	m_QAll = combination; 
	m_QCurrent *= m_QAll;
	m_QCurrent.toEuler(m_Rot);
	m_Rot *= core::RADTODEG;
	m_Direction = m_Rot.rotationToDirection(core::vector3df(0, 0, 1));

	m_ButtonMask = m_data.ButtonMask;
	m_Firing = (m_data.ButtonMask & INPUT_PRIMARY) != 0; 
	m_Secondary = (m_data.ButtonMask & INPUT_SECONDARY) != 0;
	m_Tertiary = (m_data.ButtonMask & INPUT_TERTIARY) != 0;
	 
	m_lock.Unlock();
}

void NetworkActuator::feed(const struct ActuatorData& data)
{
	m_lock.Lock();
	m_data = data;
	m_lock.Unlock();
}

Actuator* NetworkActuator::GetReplacement()
{	
	return new DummyActuator(m_QCurrent);
}
