#include <vector3d.h>
#include "DummyActuator.h" 
#include "SpaceObject.h"
#include "PropertyState.h"
#include "LogicConstants.h"
#include "AIController.h"

using namespace irr;

DummyActuator::DummyActuator() : Actuator()
{
}
DummyActuator::DummyActuator(irr::core::quaternion& current) : Actuator(current)
{
}
DummyActuator::~DummyActuator()
{
}

void DummyActuator::update(SpaceObject* host, const float dt)
{

	core::vector3df ForwardVector =  getRotation().rotationToDirection(core::vector3df(0, 0, 1));
	m_Thrust = 0.1f;
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
			//m_Thrust *= HostThrust->Current();
		}
	}

	m_FRoll = 0.0f * (1.0f - m_Thrust * 0.8f);
	m_FPitch = 0.0f * (1.0f - m_Thrust * 0.8f); 
	m_FYaw = 0.1f * (1.0f - m_Thrust * 0.8f); 

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

	m_Firing = false;
	m_Secondary = false;
	m_Tertiary = false;
	m_Thrust = 0;

}