#include <irrlicht.h>

#include "Actuator.h"
#include "SpaceObject.h"
#include "DummyActuator.h"
#include "LogicConstants.h"

using namespace irr;

ActuatorOutput ActuatorData::toOutput(const unsigned int id)
{
	return ActuatorOutput(id, *this);
}

Actuator::Actuator() : m_Object (NULL), m_Scene(NULL), 
	m_FPitch(0.0f), m_FRoll(0.0f), m_FYaw(0.0f), m_FThrust(0.0f), 
	m_Firing(false), m_Secondary(false), m_Tertiary(false), m_Thrust(0.0f), m_needsReplacement(false), m_ButtonMask(0)
{

}
Actuator::Actuator(core::quaternion& current) : m_Object (NULL), m_Scene(NULL), 
	m_FPitch(0.0f), m_FRoll(0.0f), m_FYaw(0.0f), m_FThrust(0.0f), 
	m_Firing(false), m_Secondary(false), m_Tertiary(false), m_Thrust(0.0f), m_needsReplacement(false), m_QCurrent(current), m_ButtonMask(0)
{

}

Actuator::~Actuator()
{

}

int Actuator::create(irr::scene::IMeshSceneNode* inNode, irr::scene::ISceneManager* inScene)
{
	if (inNode)
	{
		m_Object = inNode;
		m_Pos = m_Object->getPosition();
		m_Rot = m_Object->getRotation();
		m_QCurrent.set(m_Rot*core::DEGTORAD);
	}

	if (inScene) m_Scene = inScene;
	else return -1; 

	return 1; 
}

void Actuator::update(SpaceObject* host, const float dt)
{
}

Actuator* Actuator::GetReplacement()
{
	return new DummyActuator;
}

bool Actuator::NeedsReplacement() const
{
	return m_needsReplacement;
}

void Actuator::SetNeedReplacement(const bool b)
{
	m_needsReplacement = b;
}

bool Actuator::CheckButton(unsigned int value)
{
	return ((m_ButtonMask & value) != 0); 
}

bool Actuator::CheckActType(unsigned short type)
{
	bool Output = false;
	switch (type)
	{
	case ACT_PRIMARY:
		Output = ((m_ButtonMask & INPUT_PRIMARY) != 0);
		break;
	case ACT_SECONDARY:
		Output = ((m_ButtonMask & INPUT_SECONDARY) != 0);
		break;
	case ACT_TERTIARY:
		Output = ((m_ButtonMask & INPUT_TERTIARY) != 0);
		break;
	default:
		break;
	}
	return Output; 
}