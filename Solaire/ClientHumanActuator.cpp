#include "ClientHumanActuator.h"
#include "TCPPacket.h"
#include "TCPPacketFactory.h"
#include "NetworkController.h"
#include "LogicConstants.h"

ClientHumanActuator::ClientHumanActuator(const unsigned int id) : m_id(id), m_currentOutput(id)
{
}

ClientHumanActuator::~ClientHumanActuator()
{
}

void ClientHumanActuator::update(SpaceObject*, const float dt)
{
	m_currentOutput = basicUpdate(dt).toOutput(m_id);
	m_Firing = (m_currentOutput.ButtonMask & INPUT_PRIMARY) != 0;
	
	TCPPacket* packet = TCPPacketFactory::get().createPacket(CLIENT_ACTUATOR_DATA, &m_currentOutput, 0);
	if(packet)
	{
		NetworkController::get().sendPacket(packet);
		delete packet;
	}
}

const ActuatorOutput& ClientHumanActuator::getCurrent() const
{
	return m_currentOutput;
}
