#define WIN64_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <ws2tcpip.h>
#include <cassert>

#include <irrlicht.h>

#include "TCPPacket.h"
#include "System.h"
#include "NetworkUtilities.h"
#include "NetworkController.h"
#include "LANServer.h"
#include "NamePacketHelper.h"
#include "LANFinalView.h"
#include "ChatHelper.h"
#include "GUIConstants.h"
#include "SpaceObjectNetworkInfo.h"
#include "System.h"
#include "NetworkActuator.h"
#include "SpaceObject.h"
#include "SpaceObjectShell.h"
#include "LogicScene.h"
#include "LANServerScene.h"
#include "SpaceObjectFactory.h"
#include "LANGameCreationHelper.h"
#include "ScoreUpdateHelper.h"

using namespace irr;

//Bounds helper for parsing received packets: returns true only if reading/writing
//`len` bytes starting at `offset` stays within a buffer of `total` bytes. Written to be
//safe against unsigned overflow in `offset + len`. Every parser below that uses a length
//or count taken from the wire must gate its memcpy/allocation through this so a corrupt
//field can't read past the (already fully received) m_data buffer.
static inline bool fitsWithin(unsigned int total, unsigned int offset, unsigned int len)
{
	return offset <= total && len <= (total - offset);
}

int TCPPacket::receiveHeader(SOCKET s, char dest[TCP_HEADER_SIZE])
{
	if(s != INVALID_SOCKET)
	{
		unsigned int receivedSoFar = 0;
		while(receivedSoFar < TCP_HEADER_SIZE)
		{
			int retVal = recv(s, dest+receivedSoFar, TCP_HEADER_SIZE-receivedSoFar, 0); //first get size and type of data 
			if(retVal == SOCKET_ERROR)
			{
				retVal = WSAGetLastError();
				if(retVal != WSAEWOULDBLOCK)
				{
					return RECEIVE_ERROR;
				}
				else
				{
					if(receivedSoFar == 0)
					{
						return CAN_SKIP_CYCLE;
					}
				}
			}
			else if(retVal == 0)
			{
				return RECEIVED_EOF;
			}
			else
			{
				receivedSoFar += retVal;
			}
		}

		if(receivedSoFar == TCP_HEADER_SIZE)
		{
			return RECEIVED_HEADER_SUCCESSFULLY;
		}
		else
		{
			return RECEIVED_WRONG_SIZE_HEADER;
		}
	}
	return NO_VALID_SOCKET;
}

TCPPacket::TCPPacket() : m_size(0), m_data(NULL), m_initialized(false)
{
}

TCPPacket::~TCPPacket()
{
	if(m_data)
	{
		delete[] m_data;
		m_data = NULL;
	}
}

bool TCPPacket::init(void* data, const unsigned int){
	m_initialized = true;
	return true;
}

bool TCPPacket::send(SOCKET s)
{
	if(m_initialized && s != INVALID_SOCKET)
	{
#ifdef _DEBUG
		core::stringc infoMsg("sending packet type ");
		infoMsg+=getType();
		infoMsg+=", size=";
		infoMsg+=m_size;
		System::get().log(infoMsg.c_str());
#endif

		unsigned int sentSoFar = 0;
		while(sentSoFar < m_size)
		{
			int retVal = ::send(s, m_data+sentSoFar, m_size-sentSoFar, 0);
			if(retVal == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if(error != WSAEWOULDBLOCK)
				{
					core::stringc errorMsg("send() failed with ");
					errorMsg+=getSocketError(WSAGetLastError());
					errorMsg+=". Resetting connections.";
					System::get().log(errorMsg.c_str());
					return false;
				}
#ifdef _DEBUG
				else
				{
					core::stringc info("send() would block for type ");
					info+=getType();
					System::get().log(info.c_str());
				}
#endif
			}
			else
			{
				sentSoFar+=retVal;
			}
		}

		if(sentSoFar != m_size)
		{
			core::stringc errorMsg("we didn't send everything. we only sent ");
			errorMsg+=sentSoFar;
			errorMsg+=" bytes out of ";
			errorMsg+=m_size;
			System::get().log(errorMsg.c_str());
			return false;
		}
		else
		{
#ifdef _DEBUG
			core::stringc infoMsg("successfully sent everything (");
			infoMsg+=m_size;
			infoMsg+=" bytes )";
			System::get().log(infoMsg.c_str());
#endif
			return true;
		}
	}
	return false;
}

bool TCPPacket::receive(SOCKET s)
{
	//m_size was already validated against [TCP_HEADER_SIZE, MAX_TCP_PACKET_SIZE] when the
	//packet was created from the header, but guard again here so this is safe even if
	//called another way: a size below the header would overflow the header stamp below,
	//and an oversized one would be a hostile/garbage allocation.
	if(m_initialized && s != INVALID_SOCKET && m_size >= TCP_HEADER_SIZE && m_size <= MAX_TCP_PACKET_SIZE)
	{
		if(m_data)
		{
			delete[] m_data;
			m_data = NULL;
		}
		m_data = new char[m_size];
			
		memset(m_data, 0, m_size);
		TCPPacketType type = getType();
		memcpy(m_data, &m_size, 4);
		memcpy(m_data+4, &type, 4);
		int retVal = 0;
		unsigned int readSoFar = TCP_HEADER_SIZE; //we assume we've read the header
		while(readSoFar < m_size)
		{
			retVal = recv(s, m_data + readSoFar, m_size-readSoFar, 0);
			if(retVal == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if(error != WSAEWOULDBLOCK)
				{
					core::stringc errorMsg("recv() failed with ");
					errorMsg+=getSocketError(WSAGetLastError());
					System::get().log(errorMsg.c_str());
					return false;
				}
			}
			else if(retVal == 0)
			{
				//eof, close connection
				core::stringc errorMsg("closing connection (received EOF)");
				System::get().log(errorMsg.c_str());
				return false;
			}
			else
			{
				readSoFar+=retVal;
			}
		}
		return readSoFar == m_size;
	}
	return false;
}

SendNamePacket::SendNamePacket() : TCPPacket()
{

}

SendNamePacket::~SendNamePacket()
{

}

TCPPacketType SendNamePacket::getType() const
{
	return SEND_NAME;
}

bool SendNamePacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		TCPPacketType type = getType();
		
		wchar_t* playerName = static_cast<wchar_t*>(data);
		size_t playerNameSize;
		wcstombs_s(&playerNameSize, NULL, 0, playerName, _TRUNCATE);
		char* cplayerName = new char[playerNameSize];
		wcstombs_s(&playerNameSize, cplayerName, playerNameSize, playerName, _TRUNCATE);

		m_size = TCP_HEADER_SIZE + playerNameSize;
		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);
			memcpy(m_data+4+4, cplayerName, playerNameSize);
			delete[] cplayerName;
		}
		else
		{
			delete[] cplayerName;
			return false;
		}
	}
	return TCPPacket::init(m_data, m_size);
}

bool SendNamePacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool SendNamePacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		if(m_size > TCP_HEADER_SIZE)
		{
			const unsigned int playerNameLength = m_size-TCP_HEADER_SIZE;
			char* playerName = new char[playerNameLength+1];
			memset(playerName, 0, playerNameLength+1);
			if(TCPPacket::receive(s))
			{
				memcpy(playerName, m_data+TCP_HEADER_SIZE, playerNameLength);
				LANServer* server = NetworkController::get().getServer();
				if(server)
				{
					server->checkName(s, playerName);
					returnValue = true;
				}
			}
			delete[] playerName;
		}
	}
	return returnValue;
}

AcceptNamePacket::AcceptNamePacket() : TCPPacket()
{

}

AcceptNamePacket::~AcceptNamePacket()
{

}

TCPPacketType AcceptNamePacket::getType() const
{
	return ACCEPT_NAME;
}

bool AcceptNamePacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		m_size = TCP_HEADER_SIZE;
		TCPPacketType type = getType();
		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);
		}
		else
			return false;
	}
	return TCPPacket::init(m_data, m_size);
}

bool AcceptNamePacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool AcceptNamePacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		//Always drain the declared body off the socket FIRST, then validate. The old code
		//validated m_size before draining, so a wrong-sized packet left its body bytes in
		//the stream and desynced every packet after it. Once the body is consumed it is
		//safe to reject on a size mismatch without corrupting the stream.
		if(TCPPacket::receive(s) && m_size == TCP_HEADER_SIZE)
		{
			returnValue = true;
		}
	}
	return returnValue;
}

RefreshNamesPacket::RefreshNamesPacket() : TCPPacket()
{

}

RefreshNamesPacket::~RefreshNamesPacket()
{

}

TCPPacketType RefreshNamesPacket::getType() const
{
	return REFRESH_NAME_LIST;
}

bool RefreshNamesPacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		m_size = TCP_HEADER_SIZE;
		TCPPacketType type = getType();

		NameHelper* helper = static_cast<NameHelper*>(data);
		std::vector<irr::core::stringc> teamA, teamB;
		unsigned int teamASize = 0;
		for(std::vector<irr::core::stringw>::const_iterator it = helper->teamA.begin(); it != helper->teamA.end(); ++it)
		{
			const wchar_t* orig = (*it).c_str();
			size_t nameSize;
			wcstombs_s(&nameSize, NULL, 0, orig, _TRUNCATE);//nameSize includes NULL terminator char byte
			char* name = new char[nameSize];
			wcstombs_s(&nameSize, name, nameSize, orig, _TRUNCATE);
			teamA.push_back(name);
			teamASize+=nameSize;
			teamASize+=4;//because we'll store the size of this name as an unsigned int
			delete[] name;
		}

		unsigned int teamBSize = 0;
		for(std::vector<irr::core::stringw>::const_iterator it = helper->teamB.begin(); it != helper->teamB.end(); ++it)
		{
			const wchar_t* orig = (*it).c_str();
			size_t nameSize;
			wcstombs_s(&nameSize, NULL, 0, orig, _TRUNCATE);//nameSize includes NULL terminator char byte
			char* name = new char[nameSize];
			wcstombs_s(&nameSize, name, nameSize, orig, _TRUNCATE);
			teamB.push_back(name);
			teamBSize+=nameSize;
			teamBSize+=4;//because we'll store the size of this name as an unsigned int
			delete[] name;
		}

		m_size+=8; //4 bytes for teamA's size, 4 bytes for teamB's size
		m_size+=teamASize+teamBSize;
		//m_size+=teamASize+teamBSize+4*(helper->teamA.size()+helper->teamB.size());

		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);
			memcpy(m_data+4+4, (char*)&teamASize, 4);
			memcpy(m_data+4+4+4, (char*)&teamBSize, 4);
			unsigned int sizeOffset = 0;
			for(std::vector<irr::core::stringc>::const_iterator it = teamA.begin(); it != teamA.end(); ++it)
			{
				unsigned int theSize = (*it).size()+1;//need to include 1 byte for null termination character
				memcpy(m_data+4+4+4+4+sizeOffset, (char *)&theSize, 4);
				memcpy(m_data+4+4+4+4+sizeOffset+4, (*it).c_str(), theSize);
				sizeOffset+=4+theSize;
			}
			unsigned int teamAOffset = sizeOffset;
			sizeOffset = 0;
			for(std::vector<irr::core::stringc>::const_iterator it = teamB.begin(); it != teamB.end(); ++it)
			{
				unsigned int theSize = (*it).size()+1;//need to include 1 byte for null termination character
				memcpy(m_data+4+4+4+4+teamAOffset+sizeOffset, (char *)&theSize, 4);
				memcpy(m_data+4+4+4+4+teamAOffset+sizeOffset+4, (*it).c_str(), theSize);
				sizeOffset+=4+theSize;
			}
		}
		else
			return false;
	}
	return TCPPacket::init(m_data, m_size);
}

bool RefreshNamesPacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool RefreshNamesPacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		if(TCPPacket::receive(s))
		{
			//Need the two team-size headers before anything else.
			if(!fitsWithin(m_size, TCP_HEADER_SIZE, 8))
				return false;
			unsigned int teamASize = 0;
			unsigned int teamBSize = 0;
			memcpy(&teamASize, m_data+TCP_HEADER_SIZE, 4);
			memcpy(&teamBSize, m_data+TCP_HEADER_SIZE+4, 4);
			std::vector<irr::core::stringw> teamA, teamB;
			unsigned int nameSize = 0;
			unsigned int offset = 0;

			for(offset = 0; offset < teamASize;)
			{
				//bounds-check the length field and the name bytes before touching them
				if(!fitsWithin(m_size, TCP_HEADER_SIZE+offset+4+4, 4))
					break;
				memcpy(&nameSize, m_data+TCP_HEADER_SIZE+offset+4+4, 4);
				offset+=4;
				if(offset < teamASize)
				{
					if(!fitsWithin(m_size, TCP_HEADER_SIZE+4+4+offset, nameSize))
						break;
					size_t theSize;
					mbstowcs_s(&theSize, NULL, 0, m_data+TCP_HEADER_SIZE+4+4+offset, nameSize);
					wchar_t* playername = new wchar_t[theSize];
					mbstowcs_s(&theSize, playername, theSize, m_data+TCP_HEADER_SIZE+4+4+offset, nameSize);


					teamA.push_back(playername);
					delete[] playername;
					offset+=nameSize;
				}
			}

			for(unsigned int i = 0; i < teamBSize;)
			{
				if(!fitsWithin(m_size, TCP_HEADER_SIZE+4+4+offset, 4))
					break;
				memcpy(&nameSize, m_data+TCP_HEADER_SIZE+4+4+offset, 4);
				offset+=4;
				i+=4;
				if(!fitsWithin(m_size, TCP_HEADER_SIZE+4+4+offset, nameSize))
					break;
				size_t theSize;
				mbstowcs_s(&theSize, NULL, 0, m_data+TCP_HEADER_SIZE+4+4+offset, nameSize);
				wchar_t* playername = new wchar_t[theSize];
				mbstowcs_s(&theSize, playername, theSize, m_data+TCP_HEADER_SIZE+4+4+offset, nameSize);

				teamB.push_back(playername);
				delete[] playername;
				offset+=theSize;
				i+=theSize;
			}
			LANFinalView* view = NetworkController::get().getLANFinalView();
			if(view)
			{
				view->updateNames(teamA, teamB);
				returnValue = true;
			}
		}
	}
	return returnValue;
}


SendChatTextPacket::SendChatTextPacket() : TCPPacket()
{

}

SendChatTextPacket::~SendChatTextPacket()
{

}

TCPPacketType SendChatTextPacket::getType() const
{
	return SEND_CHAT_TEXT;
}

bool SendChatTextPacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		m_size = TCP_HEADER_SIZE;
		TCPPacketType type = getType();

		ChatHelper* chat = static_cast<ChatHelper*>(data);

		size_t nameSize;
		wcstombs_s(&nameSize, NULL, 0, chat->playername.c_str(), _TRUNCATE);//nameSize includes NULL terminator char byte
		char* name = new char[nameSize];
		wcstombs_s(&nameSize, name, nameSize, chat->playername.c_str(), _TRUNCATE);

		size_t textSize;
		wcstombs_s(&textSize, NULL, 0, chat->text.c_str(), _TRUNCATE);//textSize includes NULL terminator char byte
		char* text = new char[textSize];
		wcstombs_s(&textSize, text, textSize, chat->text.c_str(), _TRUNCATE);


		m_size+=8; //4 bytes for name's size, 4 bytes for text's size
		m_size+=nameSize+textSize;

		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);
			memcpy(m_data+4+4, (char*)&nameSize, 4);
			memcpy(m_data+4+4+4, (char*)&textSize, 4);

			memcpy(m_data+4+4+4+4, name, nameSize);
			memcpy(m_data+4+4+4+4+nameSize, text, textSize);
			delete[] name;
			delete[] text;		
		}
		else
		{
			delete[] name;
			delete[] text;
			return false;
		}
	}
	return TCPPacket::init(m_data, m_size);
}

bool SendChatTextPacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool SendChatTextPacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		if(TCPPacket::receive(s))
		{
			//Need the two length headers, then validate name and text bytes fit.
			if(!fitsWithin(m_size, TCP_HEADER_SIZE, 8))
				return false;
			unsigned int nameSize = 0;
			unsigned int textSize = 0;
			memcpy(&nameSize, m_data+TCP_HEADER_SIZE, 4);
			memcpy(&textSize, m_data+TCP_HEADER_SIZE+4, 4);

			if(!fitsWithin(m_size, TCP_HEADER_SIZE+4+4, nameSize) ||
			   !fitsWithin(m_size, TCP_HEADER_SIZE+4+4+nameSize, textSize))
				return false;

			size_t theSize = 0;
			mbstowcs_s(&theSize, NULL, 0, m_data+TCP_HEADER_SIZE+4+4, nameSize);
			wchar_t* playername = new wchar_t[theSize];
			mbstowcs_s(&theSize, playername, theSize, m_data+TCP_HEADER_SIZE+4+4, nameSize);

			theSize = 0;
			mbstowcs_s(&theSize, NULL, 0, m_data+TCP_HEADER_SIZE+4+4+nameSize, textSize);
			wchar_t* text = new wchar_t[theSize];
			mbstowcs_s(&theSize, text, theSize, m_data+TCP_HEADER_SIZE+4+4+nameSize, textSize);

			gui::IGUIEditBox* chat = (gui::IGUIEditBox*)System::get().getDevice()->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_LANFINAL_CHAT_EDITBOX, true);
			if(chat)
			{
				core::stringw t(chat->getText());
				t+=L"\n";
				t+=playername;
				t+=L": ";
				t+=text;
				chat->setText(t.c_str());
			}
			if(NetworkController::get().getServer())
			{
				NetworkController::get().sendPacket(this);
			}

			delete[] playername;
			delete[] text;
			returnValue = true;
		}
	}
	return returnValue;
}

UpdateSpaceObjectPacket::UpdateSpaceObjectPacket() : TCPPacket()
{

}

UpdateSpaceObjectPacket::~UpdateSpaceObjectPacket()
{

}

TCPPacketType UpdateSpaceObjectPacket::getType() const
{
	return UPDATE_SPACEOBJECT;
}

bool UpdateSpaceObjectPacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		
		m_size = TCP_HEADER_SIZE;
		TCPPacketType type = getType();
		
		//1 int for the ID, 3 floats for the position and 3 floats for the rotation, 6 floats for the health stuff, 1 char for the target type and 1 unsigned int for the target's id
		m_size+=4 + 3*4 + 3*4 + 6*4 + 1 + 4; 
		SpaceObjectNetworkInfo helper = *(SpaceObjectNetworkInfo*)data;
		
		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);
			memcpy(m_data+4+4, (char*)&helper.id, 4);

			memcpy(m_data+4+4+4, (char*)&helper.position.X, 4);
			memcpy(m_data+4+4+4+4, (char*)&helper.position.Y, 4);
			memcpy(m_data+4+4+4+4+4, (char*)&helper.position.Z, 4);

			memcpy(m_data+4+4+4+4+4+4, (char*)&helper.rotation.X, 4);
			memcpy(m_data+4+4+4+4+4+4+4, (char*)&helper.rotation.Y, 4);
			memcpy(m_data+4+4+4+4+4+4+4+4, (char*)&helper.rotation.Z, 4);
			
			//health stuff
			memcpy(m_data+4+4+4+4+4+4+4+4+4, (char*)&helper.ArmourRemaining, 4);
			memcpy(m_data+4+4+4+4+4+4+4+4+4+4, (char*)&helper.shieldRemaining, 4);
			memcpy(m_data+4+4+4+4+4+4+4+4+4+4+4, (char*)&helper.impactTime, 4);
			memcpy(m_data+4+4+4+4+4+4+4+4+4+4+4+4, (char*)&helper.hitDirection.X, 4);
			memcpy(m_data+4+4+4+4+4+4+4+4+4+4+4+4+4, (char*)&helper.hitDirection.Y, 4);
			memcpy(m_data+4+4+4+4+4+4+4+4+4+4+4+4+4+4, (char*)&helper.hitDirection.Z, 4);

			//target stuff
			memcpy(m_data+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, &helper.targetType, 1);
			memcpy(m_data+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+1, (char*)&helper.targetID, 4);
		}
		
		else
			return false;
	}
	return TCPPacket::init(m_data, m_size);}

bool UpdateSpaceObjectPacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool UpdateSpaceObjectPacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		//drain first, then validate size (see AcceptNamePacket::receive note)
		if(TCPPacket::receive(s) && m_size == TCP_HEADER_SIZE+57/*+28*/)
		{

			SpaceObjectNetworkInfo helper;
			memcpy(&(helper.id), m_data+TCP_HEADER_SIZE, 4);
			//position stuff
			memcpy(&(helper.position.X), m_data+TCP_HEADER_SIZE+4, 4);
			memcpy(&(helper.position.Y), m_data+TCP_HEADER_SIZE+4+4, 4);
			memcpy(&(helper.position.Z), m_data+TCP_HEADER_SIZE+4+4+4, 4);
			memcpy(&(helper.rotation.X), m_data+TCP_HEADER_SIZE+4+4+4+4, 4);
			memcpy(&(helper.rotation.Y), m_data+TCP_HEADER_SIZE+4+4+4+4+4, 4);
			memcpy(&(helper.rotation.Z), m_data+TCP_HEADER_SIZE+4+4+4+4+4+4, 4);
	   
			//health stuff
			memcpy(&(helper.ArmourRemaining), m_data+TCP_HEADER_SIZE+4+4+4+4+4+4+4, 4);
			memcpy(&(helper.shieldRemaining), m_data+TCP_HEADER_SIZE+4+4+4+4+4+4+4+4, 4);
			memcpy(&(helper.impactTime), m_data+TCP_HEADER_SIZE+4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(helper.hitDirection.X), m_data+TCP_HEADER_SIZE+4+4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(helper.hitDirection.Y), m_data+TCP_HEADER_SIZE+4+4+4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(helper.hitDirection.Z), m_data+TCP_HEADER_SIZE+4+4+4+4+4+4+4+4+4+4+4+4, 4);

			//target stuff
			memcpy(&(helper.targetType), m_data+TCP_HEADER_SIZE+4+4+4+4+4+4+4+4+4+4+4+4+4, 1);
			memcpy(&(helper.targetID),   m_data+TCP_HEADER_SIZE+4+4+4+4+4+4+4+4+4+4+4+4+4+1, 4);

			//Do NOT touch the scene / Irrlicht scene graph from this (network) thread.
			//Queue the update; the main thread applies it during LogicScene::update.
			LogicScene* scene = System::get().getCurrentScene();
			if(scene)
				scene->QueueNetworkUpdate(helper);
			returnValue = true;
		}
	}
	return returnValue;
}

ClientActuatorDataPacket::ClientActuatorDataPacket() : TCPPacket()
{

}

ClientActuatorDataPacket::~ClientActuatorDataPacket()
{

}

TCPPacketType ClientActuatorDataPacket::getType() const
{
	return CLIENT_ACTUATOR_DATA;
}

bool ClientActuatorDataPacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		m_size = TCP_HEADER_SIZE;
		TCPPacketType type = getType();
		m_size+=4 + 5*4; //1 int for the ID and 4 floats for PitchVal, YawVal, RollVal, ThrustVal
		ActuatorOutput helper = *(ActuatorOutput*)data;
		
		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);
			memcpy(m_data+4+4, (char*)&helper.shipID, 4);

			memcpy(m_data+4+4+4, (char*)&helper.PitchVal, 4);
			memcpy(m_data+4+4+4+4, (char*)&helper.RollVal, 4);
			memcpy(m_data+4+4+4+4+4, (char*)&helper.YawVal, 4);
			memcpy(m_data+4+4+4+4+4+4, (char*)&helper.ThrustVal, 4);
			memcpy(m_data+4+4+4+4+4+4+4, (char*)&helper.ButtonMask, 4);
		}
		else
			return false;
	}
	return TCPPacket::init(m_data, m_size);
}

bool ClientActuatorDataPacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool ClientActuatorDataPacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		//drain first, then validate size (see AcceptNamePacket::receive note)
		if(TCPPacket::receive(s) && m_size == TCP_HEADER_SIZE+24)
		{
			unsigned int id = 0;
			memcpy(&id, m_data+TCP_HEADER_SIZE, 4);

			ActuatorOutput helper(id);

			memcpy(&(helper.PitchVal), m_data+TCP_HEADER_SIZE+4, 4);
			memcpy(&(helper.RollVal), m_data+TCP_HEADER_SIZE+4+4, 4);
			memcpy(&(helper.YawVal), m_data+TCP_HEADER_SIZE+4+4+4, 4);
			memcpy(&(helper.ThrustVal), m_data+TCP_HEADER_SIZE+4+4+4+4, 4);
			memcpy(&(helper.ButtonMask), m_data+TCP_HEADER_SIZE+4+4+4+4+4, 4);

			//Queue for the main thread instead of looking up the SpaceObject here: this
			//runs on the server's network thread and the lookup/feed otherwise races the
			//main thread that owns and deletes those objects.
			LogicScene* scene = System::get().getCurrentScene();
			if(scene)
				scene->QueueActuatorData(helper);

			returnValue = true;
		}
	}
	return returnValue;
}

CreateSpaceObjectPacket::CreateSpaceObjectPacket() : TCPPacket()
{

}

CreateSpaceObjectPacket::~CreateSpaceObjectPacket()
{

}

TCPPacketType CreateSpaceObjectPacket::getType() const
{
	return CREATE_SPACEOBJECT;
}

bool CreateSpaceObjectPacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		m_size = TCP_HEADER_SIZE;
		TCPPacketType type = getType();
		SpaceObjectShell* shell = (SpaceObjectShell*)data;

		wchar_t* orig = const_cast<wchar_t*>(shell->Name.c_str());
		size_t nameSize;
		wcstombs_s(&nameSize, NULL, 0, orig, _TRUNCATE);//nameSize includes NULL terminator char byte
		char* name = new char[nameSize];
		wcstombs_s(&nameSize, name, nameSize, orig, _TRUNCATE);

		//4 bytes for the size of the name, nameSize bytes for the name, 4*4 bytes for ModuleCooldownFraction, same for ModuleId, 3*4 bytes for position rotation and shieldimpactdirection, 4 bytes for ShieldImpactTime ShieldRemaining ArmourRemaining Mask ID AgentID SubTypeID TargetID, 1 byte for NeedsCreation NeedsDeletion
		m_size+=4 + nameSize + 4*4 + 4*4 + 3*4*3 + 4 + 4 + 4 + 4 + 4 +4 + 4 + 4 + 1 + 1;
		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);
			memcpy(m_data+4+4, (char*)&nameSize, 4);
			memcpy(m_data+4+4+4, name, nameSize);
			memcpy(m_data+4+4+4+nameSize, (char*)&shell->ModuleCooldownFraction, 4*4);
			memcpy(m_data+4+4+4+nameSize+4*4, (char*)&shell->ModuleID, 4*4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4, (char*)&shell->Position.X, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4, (char*)&shell->Position.Y, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4, (char*)&shell->Position.Z, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4, (char*)&shell->Rotation.X, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4, (char*)&shell->Rotation.Y, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4, (char*)&shell->Rotation.Z, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4, (char*)&shell->ShieldImpactDirection.X, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4, (char*)&shell->ShieldImpactDirection.Y, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4, (char*)&shell->ShieldImpactDirection.Z, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4, (char*)&shell->ShieldImpactTime, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4, (char*)&shell->ShieldRemaining, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4, (char*)&shell->ArmourRemaining, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4, (char*)&shell->Mask, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4, (char*)&shell->ID, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, (char*)&shell->AgentID, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, (char*)&shell->SubTypeID, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, (char*)&shell->TargetID, 4);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, (char*)&shell->NeedsCreation, 1);
			memcpy(m_data+4+4+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+1, (char*)&shell->NeedsDeletion, 1);

			delete[] name;
		}
		else
		{
			delete[] name;
			return false;
		}
	}
	return TCPPacket::init(m_data, m_size);
}

bool CreateSpaceObjectPacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool CreateSpaceObjectPacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		if(TCPPacket::receive(s))
		{
			//Fixed-size payload that follows the variable-length name. Must stay in sync
			//with init(): ModuleCooldownFraction(4*4) + ModuleID(4*4) + 3 vector3df(3*4*3)
			//+ 8 uints/floats(4*8) + NeedsCreation/NeedsDeletion(1+1).
			const unsigned int fixedPayload = 4*4 + 4*4 + 3*4*3 + 4*8 + 1 + 1;

			unsigned int nameSize = 0;
			//nameSize is taken from the wire and used as a copy length, so validate that the
			//name and the fixed payload that follows it actually fit in the received buffer.
			if(!fitsWithin(m_size, TCP_HEADER_SIZE, 4))
				return false;
			memcpy(&nameSize, m_data+TCP_HEADER_SIZE, 4);
			if(!fitsWithin(m_size, TCP_HEADER_SIZE+4, nameSize) ||
			   !fitsWithin(m_size, TCP_HEADER_SIZE+4+nameSize, fixedPayload))
				return false;

			size_t theSize = 0;
			mbstowcs_s(&theSize, NULL, 0, m_data+TCP_HEADER_SIZE+4, nameSize);
			wchar_t* playername = new wchar_t[theSize];
			mbstowcs_s(&theSize, playername, theSize, m_data+TCP_HEADER_SIZE+4, nameSize);

			SpaceObjectShell shell;
			shell.Name = playername;

			memcpy(&(shell.ModuleCooldownFraction), m_data+TCP_HEADER_SIZE+4+nameSize, 4*4);
			memcpy(&(shell.ModuleID), m_data+TCP_HEADER_SIZE+4+nameSize+4*4, 4*4);
			memcpy(&(shell.Position.X), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4, 4);
			memcpy(&(shell.Position.Y), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4, 4);
			memcpy(&(shell.Position.Z), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4, 4);
			memcpy(&(shell.Rotation.X), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4, 4);
			memcpy(&(shell.Rotation.Y), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4, 4);
			memcpy(&(shell.Rotation.Z), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4, 4);
			memcpy(&(shell.ShieldImpactDirection.X), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4, 4);
			memcpy(&(shell.ShieldImpactDirection.Y), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4, 4);
			memcpy(&(shell.ShieldImpactDirection.Z), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(shell.ShieldImpactTime), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4, 4);		
			memcpy(&(shell.ShieldRemaining), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(shell.ArmourRemaining), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(shell.Mask), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(shell.ID), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(shell.AgentID), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(shell.SubTypeID), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(shell.TargetID), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, 4);
			memcpy(&(shell.NeedsCreation), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, 1);
			memcpy(&(shell.NeedsDeletion), m_data+TCP_HEADER_SIZE+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+1, 1);
			
			//Queue creation for the main thread instead of building Irrlicht scene nodes
			//here on the network thread (CreateObjectFromShell touches the scene graph,
			//which is not thread-safe and races the renderer / object cleanup).
			LogicScene* current = System::get().getCurrentScene();
			if(current)
				current->QueueNetworkCreation(shell);
			delete[] playername;

			returnValue = true;
		}
	}
	return returnValue;
}

CreateInitialShipsAndAgentPacket::CreateInitialShipsAndAgentPacket() : TCPPacket()
{

}

CreateInitialShipsAndAgentPacket::~CreateInitialShipsAndAgentPacket()
{

}

TCPPacketType CreateInitialShipsAndAgentPacket::getType() const
{
	return CREATE_INITIAL_SHIPS_AND_AGENT;
}

bool CreateInitialShipsAndAgentPacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		m_size = TCP_HEADER_SIZE;
		TCPPacketType type = getType();
		LANCreationHelper helper = *(LANCreationHelper*)data;
		
		m_size+=4+4+4+4;//1 unsigned int for how many shells we're sending, 1 unsigned int for the agend ID, 1 unsigned int for the mask and 1 unsigned int for the shipID

		//unfortunately it's not possible to easily know the size of all this stuff because the player names have variable length.
		//we could precompute it in the LANServerScene but then we may as well just compute it here, same thing really
		for(std::vector<SpaceObjectShell*>::const_iterator it = helper.allShips.begin(); it != helper.allShips.end(); ++it)
		{
			SpaceObjectShell* shell = *it;
			wchar_t* orig = const_cast<wchar_t*>(shell->Name.c_str());
			size_t nameSize;
			wcstombs_s(&nameSize, NULL, 0, orig, _TRUNCATE);//nameSize includes NULL terminator char byte
			char* name = new char[nameSize];
			wcstombs_s(&nameSize, name, nameSize, orig, _TRUNCATE);
			//4 bytes for the size of the name, nameSize bytes for the name, 4*4 bytes for ModuleCooldownFraction, same for ModuleId, 3*4 bytes for position rotation and shieldimpactdirection, 4 bytes for ShieldImpactTime ShieldRemaining ArmourRemaining Mask ID AgentID SubTypeID TargetID, 1 byte for NeedsCreation NeedsDeletion
			m_size+=4 + nameSize + 4*4 + 4*4 + 3*4*3 + 4 + 4 + 4 + 4 + 4 +4 + 4 + 4 + 1 + 1;
			delete[] name;
		}
		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			unsigned int howManyShells = helper.allShips.size();
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);
			memcpy(m_data+4+4, (char*)&howManyShells, 4);
			memcpy(m_data+4+4+4, (char*)&helper.agentID, 4);
			memcpy(m_data+4+4+4+4, (char*)&helper.mask, 4);
			memcpy(m_data+4+4+4+4+4, (char*)&helper.shipID, 4);

			unsigned int offset = 24; //already written m_size, type, howManyShells, agentID, mask and shipID
			for(std::vector<SpaceObjectShell*>::const_iterator it = helper.allShips.begin(); it != helper.allShips.end(); ++it)
			{
				SpaceObjectShell* shell = *it;
				wchar_t* orig = const_cast<wchar_t*>(shell->Name.c_str());
				size_t nameSize;
				wcstombs_s(&nameSize, NULL, 0, orig, _TRUNCATE);//nameSize includes NULL terminator char byte
				char* name = new char[nameSize];
				wcstombs_s(&nameSize, name, nameSize, orig, _TRUNCATE);

				memcpy(m_data+offset														, (char*)&nameSize, 4);
				memcpy(m_data+offset+4														, name, nameSize);
				memcpy(m_data+offset+4+nameSize												, (char*)&shell->ModuleCooldownFraction, 4*4);
				memcpy(m_data+offset+4+nameSize+4*4											, (char*)&shell->ModuleID, 4*4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4										, (char*)&shell->Position.X, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4									, (char*)&shell->Position.Y, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4									, (char*)&shell->Position.Z, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4								, (char*)&shell->Rotation.X, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4								, (char*)&shell->Rotation.Y, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4							, (char*)&shell->Rotation.Z, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4							, (char*)&shell->ShieldImpactDirection.X, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4						, (char*)&shell->ShieldImpactDirection.Y, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4						, (char*)&shell->ShieldImpactDirection.Z, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4					, (char*)&shell->ShieldImpactTime, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4					, (char*)&shell->ShieldRemaining, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4				, (char*)&shell->ArmourRemaining, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4				, (char*)&shell->Mask, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4			, (char*)&shell->ID, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4			, (char*)&shell->AgentID, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4		, (char*)&shell->SubTypeID, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4		, (char*)&shell->TargetID, 4);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4	, (char*)&shell->NeedsCreation, 1);
				memcpy(m_data+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+1	, (char*)&shell->NeedsDeletion, 1);
				//4 bytes for the size of the name, nameSize bytes for the name, 4*4 bytes for ModuleCooldownFraction, same for ModuleId, 3*4 bytes for position rotation and shieldimpactdirection, 4 bytes for ShieldImpactTime ShieldRemaining ArmourRemaining Mask ID AgentID SubTypeID TargetID, 1 byte for NeedsCreation NeedsDeletion
				offset += 4 + nameSize + 4*4 + 4*4 + 3*4*3 + 4 + 4 + 4 + 4 + 4 +4 + 4 + 4 + 1 + 1;
				delete[] name;
			}
		}
		else
		{
			return false;
		}
	}
	return TCPPacket::init(m_data, m_size);
}

bool CreateInitialShipsAndAgentPacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool CreateInitialShipsAndAgentPacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		if(TCPPacket::receive(s))
		{
			//Same fixed-size per-shell payload as CreateSpaceObjectPacket / init().
			const unsigned int fixedPayload = 4*4 + 4*4 + 3*4*3 + 4*8 + 1 + 1;

			LANCreationHelper* helper = new LANCreationHelper;

			unsigned int howManyShells = 0;
			unsigned int myAgentID = 0;
			unsigned int myMask = 0;
			unsigned int myShipID = 0;
			//The 4 leading uints (count, agentID, mask, shipID) must be present.
			if(!fitsWithin(m_size, TCP_HEADER_SIZE, 16))
			{
				delete helper;
				return false;
			}
			memcpy(&howManyShells, m_data+TCP_HEADER_SIZE, 4);
			memcpy(&myAgentID, m_data+TCP_HEADER_SIZE+4, 4);
			memcpy(&myMask, m_data+TCP_HEADER_SIZE+4+4, 4);
			memcpy(&myShipID, m_data+TCP_HEADER_SIZE+4+4+4, 4);

			helper->agentID = myAgentID;
			helper->shipID = myShipID;
			helper->mask = myMask;

			bool ok = true;
			unsigned int offset = 16;//already read howManyShells, agentID, mask and shipID
			for(unsigned int i = 0; i < howManyShells; ++i)
			{
				unsigned int nameSize = 0;
				//Validate this shell's name length and fixed payload against the buffer
				//before reading any of it (count + lengths are all off the wire).
				if(!fitsWithin(m_size, TCP_HEADER_SIZE+offset, 4))
				{
					ok = false;
					break;
				}
				memcpy(&nameSize, m_data+TCP_HEADER_SIZE+offset, 4);
				if(!fitsWithin(m_size, TCP_HEADER_SIZE+offset+4, nameSize) ||
				   !fitsWithin(m_size, TCP_HEADER_SIZE+offset+4+nameSize, fixedPayload))
				{
					ok = false;
					break;
				}

				SpaceObjectShell* shell = new SpaceObjectShell;

				size_t theSize = 0;
				mbstowcs_s(&theSize, NULL, 0, m_data+TCP_HEADER_SIZE+offset+4, nameSize);
				wchar_t* playername = new wchar_t[theSize];
				mbstowcs_s(&theSize, playername, theSize, m_data+TCP_HEADER_SIZE+offset+4, nameSize);

				shell->Name = playername;

				memcpy(&(shell->ModuleCooldownFraction), m_data+TCP_HEADER_SIZE+offset+4+nameSize, 4*4);
				memcpy(&(shell->ModuleID), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4, 4*4);
				memcpy(&(shell->Position.X), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4, 4);
				memcpy(&(shell->Position.Y), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4, 4);
				memcpy(&(shell->Position.Z), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4, 4);
				memcpy(&(shell->Rotation.X), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4, 4);
				memcpy(&(shell->Rotation.Y), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4, 4);
				memcpy(&(shell->Rotation.Z), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4, 4);
				memcpy(&(shell->ShieldImpactDirection.X), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4, 4);
				memcpy(&(shell->ShieldImpactDirection.Y), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4, 4);
				memcpy(&(shell->ShieldImpactDirection.Z), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4, 4);
				memcpy(&(shell->ShieldImpactTime), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4, 4);		
				memcpy(&(shell->ShieldRemaining), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4, 4);
				memcpy(&(shell->ArmourRemaining), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4, 4);
				memcpy(&(shell->Mask), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4, 4);
				memcpy(&(shell->ID), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4, 4);
				memcpy(&(shell->AgentID), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, 4);
				memcpy(&(shell->SubTypeID), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, 4);
				memcpy(&(shell->TargetID), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, 4);
				memcpy(&(shell->NeedsCreation), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, 1);
				memcpy(&(shell->NeedsDeletion), m_data+TCP_HEADER_SIZE+offset+4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+1, 1);

				helper->allShips.push_back(shell);

				offset+=4+nameSize+4*4+4*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+1+1;
				delete[] playername;
			}

			if(!ok)
			{
				//Truncated/corrupt packet: free the shells parsed so far (LANCreationHelper
				//owns nothing on its own) and the helper, then bail. The body was already
				//fully drained above, so returning here does not desync the stream.
				for(std::vector<SpaceObjectShell*>::const_iterator it = helper->allShips.begin(); it != helper->allShips.end(); ++it)
					delete *it;
				delete helper;
				return false;
			}

			System::get().feedDataToLANCLient(helper);

			LANFinalView* view = NetworkController::get().getLANFinalView();
			assert(view != NULL);
			if(view)
			{
				view->switchToLANScene();
			}
			returnValue = true;
		}
	}
	return returnValue;
}

DeleteSpaceObjectPacket::DeleteSpaceObjectPacket() : TCPPacket()
{

}

DeleteSpaceObjectPacket::~DeleteSpaceObjectPacket()
{

}

TCPPacketType DeleteSpaceObjectPacket::getType() const
{
	return DELETE_SPACEOBJECT;
}

bool DeleteSpaceObjectPacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		m_size = TCP_HEADER_SIZE;
		TCPPacketType type = getType();
		m_size+=4; //1 unsigned int for the object ID
		unsigned int objectID = *(unsigned int*)data;
		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);
			memcpy(m_data+4+4, (char*)&objectID, 4);
		}
		else
			return false;
	}
	return TCPPacket::init(m_data, m_size);
}

bool DeleteSpaceObjectPacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool DeleteSpaceObjectPacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		//drain first, then validate size (see AcceptNamePacket::receive note)
		if(TCPPacket::receive(s) && m_size == TCP_HEADER_SIZE+4)
		{
			unsigned int id = 0;
			memcpy(&id, m_data+TCP_HEADER_SIZE, 4);
			//Queue the deletion; the main thread looks up and flags the object during
			//LogicScene::update. Looking it up here would race the main thread's cleanup.
			LogicScene* scene = System::get().getCurrentScene();
			if(scene)
				scene->QueueNetworkDeletion(id);
			returnValue = true;
		}
	}
	return returnValue;
}


UpdateScorePacket::UpdateScorePacket() : TCPPacket()
{

}

UpdateScorePacket::~UpdateScorePacket()
{

}

TCPPacketType UpdateScorePacket::getType() const
{
	return UPDATE_SCORE;
}

bool UpdateScorePacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		m_size = TCP_HEADER_SIZE;
		TCPPacketType type = getType();
		vector<ScoreRow> scores = *(vector<ScoreRow>*)data;

		m_size+=scores.size()*21; //21 is the number of bytes for one ScoreRow
		
		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);

			unsigned int offset = 0;
			for(std::vector<ScoreRow>::const_iterator it = scores.begin(); it != scores.end(); ++it)
			{
				ScoreRow score = *it;
				memcpy(m_data+4+4+offset, (char*)&score.ID, 4);
				memcpy(m_data+4+4+offset+4, (char*)&score.Score, 4);
				memcpy(m_data+4+4+offset+4+4, (char*)&score.Kills, 4);
				memcpy(m_data+4+4+offset+4+4+4, (char*)&score.Deaths, 4);
				memcpy(m_data+4+4+offset+4+4+4+4, (char*)&score.Accuracy, 4);
				memcpy(m_data+4+4+offset+4+4+4+4+4, (char*)&score.IsTeam, 1);
				offset+=21;
			}
		}
		else
			return false;
	}
	return TCPPacket::init(m_data, m_size);}

bool UpdateScorePacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool UpdateScorePacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		if(TCPPacket::receive(s))
		{
			unsigned int numberScores = (m_size-TCP_HEADER_SIZE)/21;
			unsigned int offset = TCP_HEADER_SIZE;
			std::vector<ScoreRow> scores;
			for(unsigned int i = 0; i < numberScores; ++i)
			{
				unsigned int id;
				int score;
				unsigned int kills;
				unsigned int deaths;
				float accuracy;
				char isTeam;//0 for player data, anything else for team score

				memcpy(&id, m_data+offset, 4);
				memcpy(&score, m_data+offset+4, 4);
				memcpy(&kills, m_data+offset+4+4, 4);
				memcpy(&deaths, m_data+offset+4+4+4, 4);
				memcpy(&accuracy, m_data+offset+4+4+4+4, 4);
				memcpy(&isTeam, m_data+offset+4+4+4+4+4, 1);

				offset+=21;
				ScoreRow scoreRow(id, score, kills, deaths, accuracy, isTeam);
				scores.push_back(scoreRow);
			}
			System::get().getCurrentScene()->updateScoreboard(scores);
			returnValue = true;
		}
	}
	return returnValue;
}


ToggleRequestScorePacket::ToggleRequestScorePacket() : TCPPacket()
{

}

ToggleRequestScorePacket::~ToggleRequestScorePacket()
{

}

TCPPacketType ToggleRequestScorePacket::getType() const
{
	return TOGGLE_REQUEST_SCORE_UPDATES;
}

bool ToggleRequestScorePacket::init(void* data, const unsigned int)
{
	if(!m_initialized)
	{
		if(m_data)
		{
			delete[] m_data;
		}
		m_size = TCP_HEADER_SIZE;
		TCPPacketType type = getType();
		m_data = new (std::nothrow) char[m_size];
		if(m_data)
		{
			memcpy(m_data, (char*)&m_size, 4);
			memcpy(m_data+4, (char*)&type, 4);
		}
		else
			return false;
	}
	return TCPPacket::init(m_data, m_size);
}

bool ToggleRequestScorePacket::send(SOCKET s)
{
	return TCPPacket::send(s);
}

bool ToggleRequestScorePacket::receive(SOCKET s)
{
	bool returnValue = false;
	if(m_initialized && s != INVALID_SOCKET)
	{
		//drain first, then validate size (see AcceptNamePacket::receive note)
		if(TCPPacket::receive(s) && m_size == TCP_HEADER_SIZE)
		{
			//server will typically receive this packet
			LANServer* server = NetworkController::get().getServer();
			if(server)
			{
				server->toggleWantScore(s);
			}
			returnValue = true;
		}
	}
	return returnValue;
}


