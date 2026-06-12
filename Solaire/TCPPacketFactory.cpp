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
		case REJECT_GAME_FULL:
		{
			p = new (std::nothrow) RejectGameFullPacket;
			break;
		}
		case SYSTEM_MESSAGE:
		{
			p = new (std::nothrow) SystemMessagePacket;
			break;
		}
		case PLAYER_READY:
		{
			p = new (std::nothrow) ClientReadyPacket;
			break;
		}
		case KILL_FEED:
		{
			p = new (std::nothrow) KillFeedPacket;
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
			//Unknown/garbage type. This used to `throw 0`, but the throw propagated
			//uncaught out of the network thread's run() and called std::terminate(),
			//crashing the whole game on a single corrupt byte. Return NULL instead and
			//let the caller drop the connection.
			p = NULL;
			break;
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

	//Validate the size that came off the wire BEFORE trusting it. A size smaller than the
	//header would cause an out-of-bounds write while stamping the header into the buffer;
	//a wildly large size would cause a huge/failed allocation. Either means the stream is
	//corrupt or desynced, so refuse to build a packet and let the caller drop the link.
	if(size < TCP_HEADER_SIZE || size > MAX_TCP_PACKET_SIZE)
	{
		return NULL;
	}

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

