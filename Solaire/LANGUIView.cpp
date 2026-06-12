#include <ws2tcpip.h>

#include <irrlicht.h>

#include "MenuScene.h"
#include "LANGUIView.h"
#include "System.h"
#include "GUIConstants.h"
#include "NetworkController.h"
#include "GameListener.h"
#include "NetworkUtilities.h"
#include "TCPPacket.h"
#include "TCPPacketFactory.h"

using namespace irr;

LANGUIView::LANGUIView(MenuScene* parent) : m_socket(INVALID_SOCKET), GUIView(parent), m_listener(new GameListener(parent))
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
	core::dimension2d<u32> dim = driver->getScreenSize();

	float x = 0.3f;
	float y = 0.35f;
	float tableHeight = 0.35f;

	m_gameTable = env->addTable(core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*y), static_cast<s32>(dim.Width*(0.95f)), static_cast<s32>(dim.Height*(y+tableHeight))), NULL, GUI_ID_LANMENU_TABLE, false);
	m_gameTable->addColumn(L"Name", 0);
	m_gameTable->addColumn(L"IP", 1);
	m_gameTable->addColumn(L"Players", 2);
	m_gameTable->setColumnWidth(0, static_cast<u32>(m_gameTable->getAbsoluteClippingRect().getWidth()*0.33f));
	m_gameTable->setColumnWidth(1, static_cast<u32>(m_gameTable->getAbsoluteClippingRect().getWidth()*0.33f));
	m_gameTable->setColumnWidth(2, static_cast<u32>(m_gameTable->getAbsoluteClippingRect().getWidth()*0.33f));
	m_gameTable->setColumnOrdering(0, gui::EGCO_FLIP_ASCENDING_DESCENDING);
	m_gameTable->setColumnOrdering(1, gui::EGCO_NONE);
	m_gameTable->setColumnOrdering(2, gui::EGCO_FLIP_ASCENDING_DESCENDING);
	m_gameTable->setDrawFlags(gui::EGTDF_ACTIVE_ROW);
	m_gameTable->setResizableColumns(true);


	float buttonWidth = 0.2f;
	float buttonHeight = buttonWidth*0.5f;
	float deltaY = 0.05f;
	gui::IGUIButton* create = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*(y+tableHeight+deltaY)), static_cast<s32>(dim.Width*(x+buttonWidth)), static_cast<s32>(dim.Height*(y+tableHeight+deltaY+buttonHeight))), NULL, GUI_ID_LANMENU_CREATE_BUTTON, L"Create...", L"Creates a new game");	
	create->setImage(driver->getTexture("TestButtonUp.tga"));
	create->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	create->setScaleImage(true);
	create->setUseAlphaChannel(true);
	create->setDrawBorder(false);


	m_joinButton = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*(x+buttonWidth+0.05f)), static_cast<s32>(dim.Height*(y+tableHeight+deltaY)), static_cast<s32>(dim.Width*(x+buttonWidth+0.05f+buttonWidth)), static_cast<s32>(dim.Height*(y+tableHeight+deltaY+buttonHeight))), NULL, GUI_ID_LANMENU_JOIN_BUTTON, L"Join", L"Joins the selected game");
	m_joinButton->setImage(driver->getTexture("TestButtonUp.tga"));
	m_joinButton->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	m_joinButton->setScaleImage(true);
	m_joinButton->setUseAlphaChannel(true);
	m_joinButton->setDrawBorder(false);
	m_joinButton->setEnabled(false);



	gui::IGUIButton* test = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*(x+2*(buttonWidth+0.05f))), static_cast<s32>(dim.Height*(y+tableHeight+deltaY)), static_cast<s32>(dim.Width*(x+2*(buttonWidth+0.05f)+buttonWidth)), static_cast<s32>(dim.Height*(y+tableHeight+deltaY+buttonHeight))), NULL, GUI_ID_LAN_TEST_BUTTON, L"Test", L"joins a local game on 127.0.0.1");
	test->setImage(driver->getTexture("TestButtonUp.tga"));
	test->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	test->setScaleImage(true);
	test->setUseAlphaChannel(true);
	test->setDrawBorder(false);
	
	//Second row: "Join IP..." lets you connect to a game by typing its address directly,
	//useful when discovery can't see it (different subnet, etc.) or for quick local testing.
	float row2Y = y+tableHeight+deltaY+buttonHeight+0.02f;
	gui::IGUIButton* joinIP = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*row2Y), static_cast<s32>(dim.Width*(x+buttonWidth)), static_cast<s32>(dim.Height*(row2Y+buttonHeight))), NULL, GUI_ID_LAN_JOINIP_BUTTON, L"Join IP...", L"Join a game by typing its IP address");
	joinIP->setImage(driver->getTexture("TestButtonUp.tga"));
	joinIP->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	joinIP->setScaleImage(true);
	joinIP->setUseAlphaChannel(true);
	joinIP->setDrawBorder(false);

	m_elements.push_back(m_gameTable);
	m_elements.push_back(create);
	m_elements.push_back(m_joinButton);
	m_elements.push_back(test);
	m_elements.push_back(joinIP);

	hide();
	m_listener->setPaused(true);
	m_listener->start();
}

LANGUIView::~LANGUIView()
{
	hide();
	if(m_listener)
	{
		delete m_listener;
	}
}

void LANGUIView::show()
{	
	GUIView::show();
	m_listener->setPaused(false);
}

void LANGUIView::hide()
{

	GUIView::hide();
	m_listener->setPaused(true);
}

void LANGUIView::registerNewGame(wchar_t* gameName, wchar_t* ip, const unsigned int numberPlayers)
{
	bool found = false;
	for(int i = 0; i < m_gameTable->getRowCount(); ++i)
	{
		if(!_wcsicmp(gameName, m_gameTable->getCellText(i, 0)) && !_wcsicmp(ip, m_gameTable->getCellText(i, 1)))
		{
			m_gameTable->setCellText(i, 2, core::stringw(numberPlayers), video::SColor(255, 255, 255, 255));
			found = true;
			return;
		}	
	}
	if(!found)
	{
		unsigned int r = m_gameTable->addRow(m_gameTable->getRowCount());
		m_gameTable->setCellText(r, 0, gameName, video::SColor(255, 255, 255, 255));
		m_gameTable->setCellText(r, 1, ip, video::SColor(255, 255, 255, 255));
		m_gameTable->setCellText(r, 2, core::stringw(numberPlayers), video::SColor(255, 255, 255, 255));
		//m_gameTable->OnPostRender(1);
		m_joinButton->setEnabled(true);
	}
}

void LANGUIView::unregisterNewGame(wchar_t* gameName, wchar_t* ip)
{
	for(int i = 0; i < m_gameTable->getRowCount(); ++i)
	{
		if(!_wcsicmp(gameName, m_gameTable->getCellText(i, 0)) && !_wcsicmp(ip, m_gameTable->getCellText(i, 1)))
		{
			m_gameTable->removeRow(i);
			if(m_gameTable->getRowCount() < 1)
				m_joinButton->setEnabled(false);
			return;
		}	
	}
}

void LANGUIView::stopListening()
{
	m_listener->setPaused(true);
}

int LANGUIView::connect(const wchar_t* gameIP, const wchar_t* playerName)
{
	addrinfo hints = { 0 };   
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	size_t gameNameSize;
	wcstombs_s(&gameNameSize, NULL, 0, gameIP, _TRUNCATE);
	char* gameName = new char[gameNameSize];
	wcstombs_s(&gameNameSize, gameName, gameNameSize, gameIP, _TRUNCATE);

	char portNumber[6];
	sprintf_s(portNumber, "%hu\0", 9171);

	addrinfo* serverInfo;
	int status = getaddrinfo(gameName, portNumber, &hints, &serverInfo);

	int returnCode = -1;

	if(status == 0)
	{
		m_socket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
		if(m_socket == INVALID_SOCKET)
		{
			int theError = WSAGetLastError();
			core::stringc errorMsg("Create socket failed with ");
			errorMsg += getSocketError(theError);
			System::get().log(errorMsg.c_str());
			closesocket(m_socket);
			returnCode = 2;
		}
		else
		{
			int theError = ::connect(m_socket, serverInfo->ai_addr, serverInfo->ai_addrlen);
			if(theError == SOCKET_ERROR)
			{
				theError = WSAGetLastError();
				core::stringc errorMsg("failed to connect to server with ");
				errorMsg += getSocketError(theError);
				System::get().log(errorMsg.c_str());
				closesocket(m_socket);
				returnCode = 3;
			}
			else
			{
				//we are connected, send our nickname to see if it's accepted
				TCPPacket* packet = TCPPacketFactory::get().createPacket(SEND_NAME, const_cast<wchar_t*>(playerName), 0);
				if(packet)
				{
					if(!packet->send(m_socket))
					{
						System::get().log("coudn't send our name accross");
						returnCode = 4;
					}
					else
					{
						//now we need to check from the server how it responded
						char header[8];
						if(TCPPacket::receiveHeader(m_socket, header) == RECEIVED_HEADER_SUCCESSFULLY)
						{
							//The server replies with a header-only packet; its type tells us
							//whether we were accepted or rejected (and why).
							TCPPacketType responseType;
							memcpy(&responseType, header+4, 4);

							if(responseType == ACCEPT_NAME)
							{
								System::get().log("received name accept packet");

								u_long iMode = 1;
								int error = ioctlsocket(m_socket, FIONBIO, &iMode);
								if(error != 0)
								{
									core::stringc errorMsg("ioctlsocket failed with ");
									errorMsg += getSocketError(WSAGetLastError());
									System::get().log(errorMsg.c_str());
									closesocket(m_socket);
									returnCode = 8;
								}
								else
								{
									System::get().log("client successfully initialized");
									returnCode = 0;
								}
							}
							else if(responseType == REJECT_GAME_FULL)
							{
								System::get().log("server says the game is full");
								closesocket(m_socket);
								returnCode = 9;
							}
							else
							{
								System::get().log("server sent an unexpected response to our name");
								closesocket(m_socket);
								returnCode = 7;
							}
						}
						else
						{
							//No response / connection closed: the name was refused (e.g. a
							//duplicate nickname), which is how the server signals that case.
							System::get().log("no name accept packet received");
							returnCode = 7;
						}
					}
					delete packet;
				}
				else
				{
					System::get().log("failed to create SEND_NAME packet");
					returnCode = 5;
				}
			}
		}
	}
	else
	{
		System::get().log("problem with getaddrinfo()");
		returnCode = 1;
	}
	freeaddrinfo((addrinfo *)serverInfo);


	delete[] gameName;
	return returnCode;
}

SOCKET LANGUIView::getSocket() const
{
	return m_socket;
}

