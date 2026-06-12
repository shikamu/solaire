#include <irrlicht.h>

#include "LANClient.h"
#include "NetworkUtilities.h"
#include "TCPPacket.h"
#include "TCPPacketFactory.h"
#include "System.h"

using namespace irr;

LANClient::LANClient(SOCKET socket, const wchar_t* name) : m_playername(name), m_socket(socket), m_paused(true), m_showServerDisconnected(false)
{
}

LANClient::~LANClient()
{
	interrupt();
	m_paused = true;
	shutdown(m_socket, SD_BOTH);
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;
	finish();
}

void LANClient::setPaused(const bool b)
{
	m_paused = b;
}

void LANClient::sendPacket(TCPPacket* packet)
{
	packet->send(m_socket);
}

const wchar_t* LANClient::getName() const
{
	return m_playername.c_str();
}

unsigned int __stdcall LANClient::run()
{
	while(!isInterrupted())
	{
		//Sleep(100);
		if(!m_paused)
		{
			receiveData();
		}
	}
	return 0;
}

void LANClient::backToLANView()
{
	if(m_showServerDisconnected)
	{
		gui::IGUIWindow* msgbox = System::get().getDevice()->getGUIEnvironment()->addMessageBox(L"Disconnected", L"The server has disconnected!", true, gui::EMBF_OK);
		msgbox->setMinSize(core::dimension2du(300, 100));
		m_showServerDisconnected = false;
		core::list<gui::IGUIElement*> children = msgbox->getChildren();
		for(core::list<gui::IGUIElement*>::ConstIterator it = children.begin(); it != children.end(); ++it)
		{
			gui::IGUIElement* elt = (*it);
			if(elt->hasType(gui::EGUIET_BUTTON))
			{
				gui::IGUIButton* but = (gui::IGUIButton*)elt;
				if(!_wcsicmp(but->getText(), L"OK"))
				{
					s32 h = but->getAbsolutePosition().getHeight();
					s32 w = but->getAbsolutePosition().getWidth();
					but->setRelativePosition(core::position2di(static_cast<s32>((msgbox->getAbsolutePosition().getWidth() - w)*0.5f), msgbox->getAbsolutePosition().getHeight()-h-5));
				}
			}
		}
	}
	closesocket(m_socket);
	interrupt();
	System::get().toggle(SCENE_MENU);
}

void LANClient::receiveData()
{
	char sizebuff[TCP_HEADER_SIZE];
	bool shouldIncrement = true;
	switch(TCPPacket::receiveHeader(m_socket, sizebuff))
	{
		case NO_VALID_SOCKET:
			//do nothing because this shouldn't happen
			break;
		case CAN_SKIP_CYCLE:
			//do nothing, this is like a Sleep()
			break;
		case RECEIVED_EOF:
		{
			System::get().log("closing connection, received EOF while trying to receive header");
			//closesocket(m_socket);
			m_showServerDisconnected = true;
			backToLANView();
			break;
		}
		case RECEIVE_ERROR:
		{
			core::stringc errorMsg("recv() failed with ");
			errorMsg+=getSocketError(WSAGetLastError());
			//closesocket(m_socket);
			m_showServerDisconnected = true;
			backToLANView();
			break;
		}
		case RECEIVED_HEADER_SUCCESSFULLY:
		{
			TCPPacket* p = TCPPacketFactory::get().createPacket(sizebuff);
			if(p)
			{
				core::stringc infoMsg("received packet successfully, type=");
				infoMsg+=p->getType();
				System::get().log(infoMsg.c_str());
				p->receive(m_socket);
				delete p;
			}
			break;
		}
		case RECEIVED_WRONG_SIZE_HEADER:
			System::get().log("received weird header, ignoring.");
			break;
	}
}
