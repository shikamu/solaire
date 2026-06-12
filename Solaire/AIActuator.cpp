#include <vector3d.h>
#include "AIActuator.h" 
#include "SpaceObject.h"
#include "PropertyState.h"
#include "LogicConstants.h"
#include "AIController.h"

using namespace irr;

AIActuator::AIActuator(const bool advanced) : Actuator(), m_AI(NULL), m_AIInitialised(false), m_advanced(advanced)
{
}
AIActuator::AIActuator(irr::core::quaternion& current, const bool advanced) : Actuator(current), m_AI(NULL), m_AIInitialised(false), m_advanced(advanced)
{
}
AIActuator::~AIActuator()
{
	delete m_AI;
}

void AIActuator::update(SpaceObject* host, const float dt)
{
	if (!m_AIInitialised)
	{
		m_AIInitialised = true;
		m_AI = new AIController(host, host->GetParentScene());
		m_AI->setAdvanced(m_advanced);
	}

	m_AI->Update(dt);

	m_FPitch = m_AI->GetActuatorData().PitchVal;
	//m_FRoll = m_AI->GetActuatorData().RollVal;
	m_FYaw = m_AI->GetActuatorData().YawVal;
	m_Thrust = m_AI->GetActuatorData().ThrustVal;
	m_ButtonMask = m_AI->GetActuatorData().ButtonMask;

	core::vector3df ForwardVector =  getRotation().rotationToDirection(core::vector3df(0, 0, 1));
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

}