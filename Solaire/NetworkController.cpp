#define WIN64_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cassert>

#include <irrlicht.h>

#include "NetworkController.h"
#include "System.h"
#include "ConfigData.h"
#include "NetworkUtilities.h"
#include "LANClient.h"
#include "LANServer.h"
#include "LANFinalView.h"
#include "LANServerGUIView.h"

using namespace irr;



NetworkController NetworkController::m_NetworkController;

NetworkController::NetworkController(): m_server(NULL), m_LANFinalview(NULL), m_client(NULL), m_serverScene(NULL), m_state(NETWORKING_IDLING), m_successfulWSAStartup(false)
{
	m_successfulWSAStartup = doWSAStartup();
}

NetworkController::~NetworkController()
{
	WSACleanup();
}

NetworkController& NetworkController::get()
{
	return m_NetworkController;
}

void NetworkController::registerLANServer(LANServer* server)
{
	unregisterLANServer();
	m_server = server;
}

void NetworkController::unregisterLANServer()
{
	if(m_server)
	{
		//m_server->interrupt();
		//m_server->finish();
		delete m_server;
		
	}
	m_server = NULL;
}

void NetworkController::registerLANClient(LANClient* client)
{
	unregisterLANClient();

	m_client = client;
}

void NetworkController::unregisterLANClient()
{
	if(m_client)
	{
		//m_client->interrupt();
		//m_client->finish();
		delete m_client;
	}
	m_client = NULL;
}

void NetworkController::registerFinalLANView(LANFinalView* view)
{
	unregisterFinalLANView();
	m_LANFinalview = view;

	if(m_server)
	{
		LANServerGUIView* test = NULL;
		if((test = dynamic_cast<LANServerGUIView*>(view)))
		{
			//TODO see if this block is still needed
			m_server->setView(test);
			m_server->setIsInLobby(true);
		}
		else
		{
			//this is called when someone first created a game and then decided not to create and is now joining a game
			registerServerScene(NULL);
			unregisterLANServer();
		}
	}
}

void NetworkController::unregisterFinalLANView()
{
	if(m_server)
	{
		m_server->setView(NULL);
		m_server->setIsInLobby(false);
	}
	if(m_LANFinalview)
	{
		delete m_LANFinalview;
	}
	m_LANFinalview = NULL;
}

void NetworkController::registerServerScene(LANServerScene* scene)
{
	m_serverScene = scene;
}

LANServer* NetworkController::getServer() const
{
	return m_server;
}

LANClient* NetworkController::getClient() const
{
	return m_client;
}

LANFinalView* NetworkController::getLANFinalView() const
{
	return m_LANFinalview;
}

LANServerScene* NetworkController::getServerScene() const
{
	return m_serverScene;
}

void NetworkController::sendPacket(TCPPacket* packet)
{
	if((m_server && m_client))
	{
		System::get().log("NetworkController::sendPacket => We have both a server and a client objects. Something's wrong");	
	}
	else if((!m_server && !m_client))
	{
		System::get().log("NetworkController::sendPacket => We have neither a server nor a client objects. Something's wrong");	
	}
	else if(m_server)
	{
		m_server->sendPacket(packet);
	}
	else if(m_client)
	{
		m_client->sendPacket(packet);
	}
}

bool NetworkController::doWSAStartup(){
	if(!m_successfulWSAStartup)
	{
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		int error = WSAStartup(wVersionRequested, &wsaData);
		if(error != 0)
		{
			m_successfulWSAStartup = false;
		}
		else
		{
			m_successfulWSAStartup = true;
		}
	}
	return m_successfulWSAStartup;
}

void NetworkController::reset()
{
	//interrupt();
	m_state = NETWORKING_IDLING;
}
