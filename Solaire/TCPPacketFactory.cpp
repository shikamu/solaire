#include <new>
#include "TCPPacketFactory.h"
#include "TCPPacket.h"

TCPPacketFactory TCPPacketFactory::m_TCPPacketFactory;

TCPPacketFactory& TCPPacketFactory::get()
{
	return m_TCPPacketFactory;
}

TCPPacket* TCPPacketFactory::instantiate(const TCPPacketType type) const
{
	TCPPacket* p = NULL;
	switch(type)
	{
		case SEND_NAME:
		{
			p = new (std::nothrow) SendNamePacket;
			break;
		}
		case ACCEPT_NAME:
		{
			p = new (std::nothrow) AcceptNamePacket;
			break;
		}
		case REFRESH_NAME_LIST:
		{
			p = new (std::nothrow) RefreshNamesPacket;
			break;
		}
		case SEND_CHAT_TEXT:
		{
			p = new (std::nothrow) SendChatTextPacket;
			break;
		}
		case UPDATE_SPACEOBJECT:
		{
			p = new (std::nothrow) UpdateSpaceObjectPacket;
			break;
		}
		case CLIENT_ACTUATOR_DATA:
		{
			p = new (std::nothrow) ClientActuatorDataPacket;
			break;
		}
		case CREATE_SPACEOBJECT:
		{
			p = new (std::nothrow) CreateSpaceObjectPacket;
			break;
		}
		case CREATE_INITIAL_SHIPS_AND_AGENT:
		{
			p = new (std::nothrow) CreateInitialShipsAndAgentPacket;
			break;
		}
		case DELETE_SPACEOBJECT:
		{
			p = new (std::nothrow) DeleteSpaceObjectPacket;
			break;
		}
		case UPDATE_SCORE:
		{
			p = new (std::nothrow) UpdateScorePacket;
			break;
		}
		case TOGGLE_REQUEST_SCORE_UPDATES:
		{
			p = new (std::nothrow) ToggleRequestScorePacket;
			break;
		}
		default:
			throw 0;//we should never get here or it means we forgot to implement a case in the switch
	}
	return p;
}

TCPPacket* TCPPacketFactory::createPacket(const TCPPacketType type, void* data, const unsigned int size) const
{
	TCPPacket* p = instantiate(type);
	if(p && p->init(data, size))
		return p;

	return NULL;
}

TCPPacket* TCPPacketFactory::createPacket(char header[TCP_HEADER_SIZE]) const
{
	unsigned int size;
	TCPPacketType type;
	memcpy(&size, header, 4);
	memcpy(&type, header+4, 4);
	TCPPacket* p = instantiate(type);
	if(p)
	{
		p->m_size = size;
		p->m_initialized = true;
	}

	return p;
}

TCPPacketFactory::TCPPacketFactory()
{
}

TCPPacketFactory::~TCPPacketFactory()
{
}

