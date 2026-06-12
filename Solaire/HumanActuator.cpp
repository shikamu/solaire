#include <cmath>
#include <irrlicht.h>

#include "HumanActuator.h"

#include "System.h"
#include "ConfigData.h"
#include "EventReceiver.h"
#include "KeyBindings.h"
#include "SpaceObject.h"
#include "PropertyState.h"
#include "LogicConstants.h"
#include "InputConstants.h"

using namespace irr;

HumanActuator::HumanActuator() : m_JoystickEnabled(false), m_AxisRange(32767.0f), m_Zoom(1.0f),
								m_InRoll (0.0f), m_InPitch(0.0f), m_InYaw(0.0f), m_InThrust(0.0f), m_InZoom (0.0f), m_RotationDampener(0.0f)
{
}

HumanActuator::~HumanActuator()
{
}

ActuatorData HumanActuator::basicUpdate(const float dt)
{

	ActuatorData output;
	static const float ThrustZeroPoint = 0.5f;
	m_hasChanged = false;

	// Update Pitch
	//////////////////////////

	float InputValue = 0.0f;

	if (System::get().getConfig()->getKeybindings().isTriggered(PITCH_MORE))
	{
		InputValue = System::get().getConfig()->getKeybindings().getValue(PITCH_MORE);
		m_hasChanged = true;
	}
	else if	(System::get().getConfig()->getKeybindings().isTriggered(PITCH_LESS))
	{
		InputValue =  -System::get().getConfig()->getKeybindings().getValue(PITCH_LESS);
		m_hasChanged = true;
	} 
	m_InPitch += (InputValue - m_InPitch) * dt * 35.0f;


	// Update Roll
	////////////////////////////

	InputValue = 0.0f;

	if (System::get().getConfig()->getKeybindings().isTriggered(ROLL_MORE))
	{
		InputValue = System::get().getConfig()->getKeybindings().getValue(ROLL_MORE);
		m_hasChanged = true;
	}
	else if	(System::get().getConfig()->getKeybindings().isTriggered(ROLL_LESS))
	{
		InputValue =  -System::get().getConfig()->getKeybindings().getValue(ROLL_LESS);
		m_hasChanged = true;
	} 
	m_InRoll += (InputValue - m_InRoll) * dt * 35.0f;

	// Update Yaw
	//////////////////////

	InputValue = 0.0f;

	if (System::get().getConfig()->getKeybindings().isTriggered(YAW_MORE))
	{
		InputValue = System::get().getConfig()->getKeybindings().getValue(YAW_MORE);
		m_hasChanged = true;
	}
	else if	(System::get().getConfig()->getKeybindings().isTriggered(YAW_LESS))
	{
		InputValue =  -System::get().getConfig()->getKeybindings().getValue(YAW_LESS);
		m_hasChanged = true;
	} 
	m_InYaw += (InputValue - m_InYaw) * dt * 35.0f;

	// Update Zoom
	//////////////////////

	InputValue = 0.0f;

	if (System::get().getConfig()->getKeybindings().isTriggered(ZOOM_IN))
	{
		InputValue = System::get().getConfig()->getKeybindings().getValue(ZOOM_IN);
		m_hasChanged = true;
	}
	else if	(System::get().getConfig()->getKeybindings().isTriggered(ZOOM_OUT))
	{
		InputValue =  -System::get().getConfig()->getKeybindings().getValue(ZOOM_OUT);
		m_hasChanged = true;
	} 
	m_InZoom += (InputValue - m_InZoom) * dt * 25.0f;
	m_Zoom = 1.0f + m_InZoom * 0.5f;

	//if(System::get().getConfig()->getKeybindings().isTriggered(ROLL_MORE))
	//{
	//	m_hasChanged = true;
	//	RollVal = System::get().getConfig()->getKeybindings().getValue(ROLL_MORE);
	//}
	//else if(System::get().getConfig()->getKeybindings().isTriggered(ROLL_LESS))
	//{
	//	m_hasChanged = true;
	//	RollVal = -System::get().getConfig()->getKeybindings().getValue(ROLL_LESS);
	//}


	//if(System::get().getConfig()->getKeybindings().isTriggered(YAW_MORE))
	//{
	//	m_hasChanged = true;
	//	YawVal = System::get().getConfig()->getKeybindings().getValue(YAW_MORE);
	//}
	//else if(System::get().getConfig()->getKeybindings().isTriggered(YAW_LESS))
	//{
	//	m_hasChanged = true;
	//	YawVal = -System::get().getConfig()->getKeybindings().getValue(YAW_LESS);
	//}
		
	//if(System::get().getConfig()->getKeybindings().isTriggered(ZOOM_IN))
	//{
	//	m_hasChanged = true;
	//	float value = System::get().getConfig()->getKeybindings().getValue(ZOOM_IN);
	//	m_Zoom = 1.0f - value * 0.5f;
	//}
	//else if(System::get().getConfig()->getKeybindings().isTriggered(ZOOM_OUT))
	//{
	//	m_hasChanged = true;
	//	float value = System::get().getConfig()->getKeybindings().getValue(ZOOM_OUT);
	//	m_Zoom = 1.0f + value * 0.5f;
	//}
	//else
	//{
	//	m_Zoom = 1.0f;
	//}

	InputValue = 0.0f;

	if (System::get().getConfig()->getKeybindings().isTriggered(THRUST_MORE))
	{
		InputValue = -System::get().getConfig()->getKeybindings().getValue(THRUST_MORE);
		m_hasChanged = true;
	}
	else if	(System::get().getConfig()->getKeybindings().isTriggered(THRUST_LESS))
	{
		InputValue =  System::get().getConfig()->getKeybindings().getValue(THRUST_LESS);
		m_hasChanged = true;
	} 
	InputValue = std::max(0.0f, ((InputValue *0.5f) + 0.5f - ThrustZeroPoint) / (1.0f - ThrustZeroPoint));

	m_InThrust += (InputValue - m_InThrust) * dt * 35.0f;
	
	output.PitchVal = m_InPitch;
	output.RollVal = m_InRoll;
	output.YawVal = m_InYaw;
	output.ThrustVal = m_InThrust;

	if (System::get().getConfig()->getKeybindings().isTriggered(ACTION_1))
	{
		output.ButtonMask |= INPUT_PRIMARY;
	}
	if (System::get().getConfig()->getKeybindings().isTriggered(ACTION_2))
	{
		output.ButtonMask |= INPUT_SECONDARY;
	}	
	if (System::get().getConfig()->getKeybindings().isTriggered(ACTION_3))
	{
		output.ButtonMask |= INPUT_TERTIARY;
	}	
	if (System::get().getConfig()->getKeybindings().isTriggered(ACTION_4))
	{
		output.ButtonMask |= INPUT_NEXT;
	}
	if (System::get().getConfig()->getKeybindings().isTriggered(ACTION_5))
	{
		output.ButtonMask |= INPUT_PREV;
	}
	if (System::get().getConfig()->getKeybindings().isTriggered(ACTION_6))
	{
		output.ButtonMask |= INPUT_FRONT;
	}
	if (System::get().getConfig()->getKeybindings().isTriggered(ACTION_7))
	{
		output.ButtonMask |= INPUT_SCORES;
	}
	if (System::get().getConfig()->getKeybindings().isTriggered(ACTION_8))
	{
		output.ButtonMask |= INPUT_PAUSE;
	}

	return output;
}

void HumanActuator::advancedUpdate(SpaceObject* host, const float dt,  const ActuatorData& basic)
{
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
	m_RotationDampener += (basic.ThrustVal - m_RotationDampener) * dt * 0.8f;
	m_FRoll = -basic.RollVal * (1.0f - m_RotationDampener * 0.8f);
	m_FPitch = -basic.PitchVal * (1.0f - m_RotationDampener * 0.8f); 
	m_FYaw = basic.YawVal * (1.0f - m_RotationDampener * 0.8f); 
	m_Thrust = basic.ThrustVal; 

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

	m_ButtonMask = basic.ButtonMask; 

	m_Firing = (basic.ButtonMask & INPUT_PRIMARY) != 0; 
	m_Secondary = (basic.ButtonMask & INPUT_SECONDARY) != 0;
	m_Tertiary = (basic.ButtonMask & INPUT_TERTIARY) != 0;
}

void HumanActuator::update(SpaceObject* host, const float dt)
{
	//ActuatorData output = basicUpdate(dt);
	advancedUpdate(host, dt, basicUpdate(dt));
}

