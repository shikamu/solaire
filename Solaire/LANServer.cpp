#include <algorithm>
#include <cassert>
#include <irrlicht.h>

#include "LANServer.h"
#include "System.h"
#include "NetworkUtilities.h"
#include "GameAdvertiser.h"
#include "LANServerGUIView.h"
#include "NetworkController.h"
#include "TCPPacketFactory.h"
#include "TCPPacket.h"
#include "NamePacketHelper.h"
#include "SpaceObjectNetworkInfo.h"
#include "SpaceObject.h"
#include "LockPointer.h"
#include "PhysicsObject.h"
#include "RenderObjectIncludes.h"
#include "SpaceObjectShell.h"
#include "LANServerScene.h"
#include "LogicConstants.h"
#include "SpaceObjectFactory.h"
#include "Agent.h"
#include "GameLog.h"

using namespace irr;

unsigned int LANServer::ID = 0;


LANServer::LANServer(const wchar_t* playerName, GameAdvertiser* advertiser, LANServerGUIView* view) : m_nickname(playerName), m_advertiser(advertiser), m_view(view), m_serverSocket(INVALID_SOCKET),
	m_serverId(0), m_initialized(false), m_paused(true), m_isInLobby(true), m_doDisconnectAll(true)
{

	m_names.push_back(m_nickname);
}

LANServer::~LANServer()
{
	m_lock.Lock();
	m_advertiser = NULL;
	interrupt();
	m_paused = true;
	m_initialized = false;

	for(std::vector<SOCKET>::const_iterator it = m_waitingForApproval.begin(); it != m_waitingForApproval.end(); ++it)
	{
		shutdown(*it, SD_BOTH);
		closesocket(*it);
	}

	for(std::map<SOCKET, irr::core::stringw>::const_iterator it = m_players.begin(); it != m_players.end(); ++it)
	{
		shutdown(it->first, SD_BOTH);
		closesocket(it->first);
	}

	shutdown(m_serverSocket, SD_BOTH);
	closesocket(m_serverSocket);
	m_serverSocket = INVALID_SOCKET;
	
	m_ShellListLock.Lock();
	for(std::vector<LockPointer<SpaceObjectShell>*>::const_iterator it = m_Shells.begin(); it != m_Shells.end(); ++it)
	{
		delete *it;
	}
	m_Shells.clear();	
	m_ShellListLock.Unlock();

	m_lock.Unlock();
	finish();
}
void LANServer::AcquireLock()
{
	//m_lock.Lock();
	m_ShellListLock.Lock();
}
void LANServer::ReleaseLock()
{
	m_ShellListLock.Unlock();
	//m_lock.Unlock();
}

void LANServer::CleanupShells()
{
	m_ShellListLock.Lock();
	for(std::vector<LockPointer<SpaceObjectShell>*>::const_iterator it = m_Shells.begin(); it != m_Shells.end(); )
	{
		SpaceObjectShell* shell = (*it)->GetPointer();
		if(shell && shell->NeedsDeletion && !shell->ServerSideObj)
		{
			delete *it;
			it = m_Shells.erase(it);
		}
		else
		{
			(*it)->Unlock();
			++it;
		}

	}
	m_ShellListLock.Unlock();
}

void LANServer::toggleWantScore(SOCKET s)
{
	m_wantScores[s] = !m_wantScores[s];
}

bool LANServer::setup()
{
	if(!m_initialized)
	{
		struct sockaddr_in serverAddr;
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddr.sin_port = htons(9171);
		m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(m_serverSocket == INVALID_SOCKET)
		{
			core::stringw errorMsg(L"Create socket failed with ");
			errorMsg += getSocketError(WSAGetLastError());
			showSetupErrorMessageBox(errorMsg.c_str());
			System::get().logw(errorMsg.c_str());
		}
		else if(bind(m_serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			core::stringw errorMsg(L"Bind failed with ");
			errorMsg += getSocketError(WSAGetLastError());
			showSetupErrorMessageBox(errorMsg.c_str());
			System::get().logw(errorMsg.c_str());
			closesocket(m_serverSocket);
		}
		else if(listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			core::stringw errorMsg(L"listen() failed with ");
			errorMsg += getSocketError(WSAGetLastError());
			showSetupErrorMessageBox(errorMsg.c_str());
			System::get().logw(errorMsg.c_str());
			closesocket(m_serverSocket);
		}
		else
		{
			u_long iMode = 1;
			int error = ioctlsocket(m_serverSocket, FIONBIO, &iMode);
			if(error != 0)
			{
				core::stringw errorMsg(L"ioctlsocket failed with ");
				errorMsg += getSocketError(WSAGetLastError());
				showSetupErrorMessageBox(errorMsg.c_str());
				System::get().logw(errorMsg.c_str());
				closesocket(m_serverSocket);
			}
			else
			{
				System::get().log("server successfully initialized");
				m_initialized = true;
			}
		}
	}
	return m_initialized;
}

void LANServer::setPaused(const bool b)
{
	m_paused = b;
}

unsigned int __stdcall LANServer::run()
{
	if(m_initialized)
	{
		while(!isInterrupted())
		{ 
			//Sleep(100);
			if(!m_paused)
			{
				m_lock.Lock();
				if(m_isInLobby)
				{
					checkConnections();
				}
				receiveData();

				//send everybody's position/rotation to everybody
				LANServerScene* scene = NetworkController::get().getServerScene();
				if(scene)
				{
					std::vector<TCPPacket*> packets;
					m_ShellListLock.Lock();
					bool deleted = false;
					for(std::vector<LockPointer<SpaceObjectShell>*>::const_iterator it = m_Shells.begin(); it != m_Shells.end(); ++it)
					{
						SpaceObjectShell* shell = (*it)->GetUnlockedPointer();
						//SpaceObjectShell* shell = (*it)->GetPointer();
						if(!shell->ServerSideObj )
							continue;

						if(shell->NeedsCreation)
						{
							deleted = false;
							TCPPacket* packet = TCPPacketFactory::get().createPacket(CREATE_SPACEOBJECT, shell, 0);
							if(packet)
							{
								packets.push_back(packet);
							}
							shell->NeedsCreation = false;
						}
						//else if(shell->NeedsDeletion && !shell->ServerSideObj)
						else if(shell->NeedsDeletion)
						//else if(shell->NeedsDeletion || !shell->ServerSideObj)
						{
							
							deleted = true;
							unsigned int id = shell->ID;

							TCPPacket* packet = TCPPacketFactory::get().createPacket(DELETE_SPACEOBJECT, &id, 0);
							if(packet)
							{
								packets.push_back(packet);
							}
							shell->ServerSideObj = NULL; 
							
						}
						else
						{
							deleted = false;
							
							/*
							SpaceObject* obj = shell->ServerSideObj;
							if(!obj || obj->NeedsDeletion())
								continue;
							
							SpaceObjectNetworkInfo p;
							p.id = obj->ID;
							//p.position = obj->GetPhysicsObject()->GetUnlockedPointer()->GetPosition();
							//p.position = obj->GetPhysicsObject()->GetPointer()->GetPosition();
							//obj->GetPhysicsObject()->Unlock();
							p.rotation = obj->GetRenderObject()->GetRotation();
							p.position = obj->GetRenderObject()->GetPosition();

							float* armour = obj->ArmourRemainingValue();
							float* shield = obj->ShieldRemainingValue();
							float* impact = obj->ShieldImpactTime();
							core::vector3df* direction = obj->ShieldImpactVector();

							p.ArmourRemaining = *armour;
							p.shieldRemaining = *shield;
							p.impactTime = *impact;
							p.hitDirection = *direction;
							


							if (obj->GetHardTarget())
							{
								p.targetType = TARGET_HARD;
								p.targetID = obj->GetHardTarget()->ID; 
							}
							else if (obj->GetSoftTarget())
							{
								p.targetType = TARGET_SOFT; 
								p.targetID = obj->GetSoftTarget()->ID; 
							}
							else
							{
								p.targetType = TARGET_NONE;
								p.targetID = 0; 
							}
							*/


							SpaceObjectNetworkInfo p;
							p.id = shell->ID;
							//p.position = obj->GetPhysicsObject()->GetUnlockedPointer()->GetPosition();
							//p.position = obj->GetPhysicsObject()->GetPointer()->GetPosition();
							//obj->GetPhysicsObject()->Unlock();
							p.rotation = shell->Rotation;
							p.position = shell->Position;

							p.ArmourRemaining = shell->ArmourRemaining;
							p.shieldRemaining = shell->ShieldRemaining;
							p.impactTime = shell->ShieldImpactTime;
							p.hitDirection = shell->ShieldImpactDirection;
							
							p.targetType = shell->TargetType;
							p.targetID = shell->TargetID;


							TCPPacket* packet = TCPPacketFactory::get().createPacket(UPDATE_SPACEOBJECT, &p, 0);
							if(packet)
							{
								packets.push_back(packet);
							}
						}
						//(*it)->Unlock();
						/*
						if(deleted)
						{
							delete *it;
							it = m_Shells.erase(it);
						}
						else ++it;
						*/
					}
					m_ShellListLock.Unlock();
					for(std::vector<TCPPacket*>::const_iterator it = packets.begin(); it != packets.end(); ++it)
					{
						TCPPacket* packet = *it;
						sendPacket(packet);
						delete packet;
					}

					if(m_wantScores.size() > 0)
					{
						vector<ScoreRow> scores;
						map<unsigned int, AgentLogData*> playerData = GameLog::Get().getAllPlayerData();
						map<unsigned int, GroupLogData*> teamData = GameLog::Get().getAllTeamData();
						for(map<unsigned int, AgentLogData*>::const_iterator it = playerData.begin(); it != playerData.end(); ++it)
						{
							scores.push_back((*it).second->toScoreRow());
						}

						for(map<unsigned int, GroupLogData*>::const_iterator it = teamData.begin(); it != teamData.end(); ++it)
						{
							scores.push_back((*it).second->toScoreRow());
						}

						TCPPacket* packet = TCPPacketFactory::get().createPacket(UPDATE_SCORE, &scores, 0);
						if(packet)
						{
							for(std::map<SOCKET, bool>::const_iterator it = m_wantScores.begin(); it != m_wantScores.end(); ++it)
							{
								if(it->second)
								{
									packet->send(it->first);
								}
							}
							delete packet;
						}
					}
				}

				if(m_approvalRemoval.size() > 0)
				{
					for(std::vector<SOCKET>::const_iterator it = m_approvalRemoval.begin(); it != m_approvalRemoval.end(); ++it)
					{
						m_waitingForApproval.erase(std::remove(m_waitingForApproval.begin(), m_waitingForApproval.end(), *it), m_waitingForApproval.end());
					}
					m_approvalRemoval.clear();
				}
				m_lock.Unlock();
			}
		}
	}
	return 0;
}

void LANServer::showSetupErrorMessageBox(const wchar_t* msg)
{

	gui::IGUIWindow* msgbox = System::get().getDevice()->getGUIEnvironment()->addMessageBox(L"Error", msg, true, gui::EMBF_OK);
	msgbox->setMinSize(core::dimension2du(300, 100));
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

void LANServer::markForApprovalRemoval(SOCKET s)
{
	m_approvalRemoval.push_back(s);
}

void LANServer::checkConnections()
{
	if(m_initialized)
	{
		sockaddr_storage clientInformation;   //holds client's IP and Port
		int clientInformationSize = sizeof(clientInformation);
		SOCKET client = accept(m_serverSocket, (sockaddr *)&clientInformation, &clientInformationSize);
		bool newClient = false;
		if(client == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			if(error == WSAEWOULDBLOCK)
			{
				//System::get().log("waiting on accept using select");


				fd_set read, err;
				TIMEVAL timeout;

				FD_ZERO(&read);
				FD_ZERO(&err);
				FD_SET(m_serverSocket, &read);
				FD_SET(m_serverSocket, &err);

				timeout.tv_sec = 0;//static_cast<int>(ConfigurationManager::get().getDouble("UDP_SERVER_TIMEOUT") - m_waitingForConnectionTimer.getTimeSec());
				timeout.tv_usec = 0;

				error = select(0, //ignored
					&read, //read
					NULL, //Write Check
					&err, //Error Check
					&timeout);


				if(error != 0)
				{
					if(FD_ISSET(m_serverSocket, &read))
					{
						//connected!
						client = accept(m_serverSocket, (sockaddr *)&clientInformation, &clientInformationSize);
						core::stringc clientIP("Connected through select: ip=");
						clientIP += inet_ntoa(((sockaddr_in*)&clientInformation)->sin_addr);
						System::get().log(clientIP.c_str());
						newClient = true;
					}
					if(FD_ISSET(m_serverSocket, &err))
					{
						//select error
						core::stringc errorMsg("select error ");
						int selectError = -1;
						int len = sizeof(selectError);
						getsockopt(m_serverSocket, SOL_SOCKET, SO_ERROR, (char*)&selectError, &len);
						errorMsg+="(error id=";
						errorMsg+=selectError;
						errorMsg+="): ";
						errorMsg+=getSocketError(selectError);
						System::get().log(errorMsg.c_str());
						closesocket(m_serverSocket);
						closesocket(client);
					}
				}
				//else
				//{
				//accept timeout :(
				//System::get().log("select timed out :(");//should happen a lot since there's no timeout and we're constantly polling
				//}
				
			}
			else
			{
				core::stringc errorMsg("accept() error: ");
				errorMsg += getSocketError(WSAGetLastError());
				System::get().log(errorMsg.c_str());
				//freeaddrinfo((addrinfo *)m_serverInfo);
				closesocket(m_serverSocket);
				closesocket(client);
			}
		}
		else
		{
			core::stringc clientIP("directly connected: ip=");
			clientIP += inet_ntoa(((sockaddr_in*)&clientInformation)->sin_addr);
			System::get().log(clientIP.c_str());
			newClient = true;
		}
		if(newClient)
		{
			

			u_long iMode = 1;
			int error = ioctlsocket(client, FIONBIO, &iMode);
			if(error != 0)
			{
				core::stringc errorMsg("ioctlsocket failed with ");
				errorMsg += getSocketError(WSAGetLastError());
				System::get().log(errorMsg.c_str());
				closesocket(client);
			}
			else
			{
				System::get().log("server successfully initialized");
							
			}


			//clients must send their names and server will check if it already exists
			m_waitingForApproval.push_back(client);
		}
	}
}

void LANServer::checkName(SOCKET s, const char* nickname)
{
	bool acceptable = true;

	core::stringc received("going to check whether received nickname '");
	received+=nickname;
	received+="' is acceptable";
	System::get().log(received.c_str());

	irr::core::stringw clientRequestedName(nickname);
	//delete[] nickname;

	for(std::vector<irr::core::stringw>::const_iterator it2 = m_names.begin(); it2 != m_names.end(); ++it2)
	{
		irr::core::stringw name = *it2;
		if(name.equals_ignore_case(clientRequestedName))
		{
			acceptable = false;
			//nope, not acceptable
			
			closesocket(s);
			//m_waitingForApproval.erase(std::remove(m_waitingForApproval.begin(), m_waitingForApproval.end(), s), m_waitingForApproval.end());
			m_approvalRemoval.push_back(s);
			break;
		}
	}
	if(acceptable)
	{
		TCPPacket* packet = TCPPacketFactory::get().createPacket(ACCEPT_NAME, NULL, TCP_HEADER_SIZE);
		if(packet->send(s))
		{
			System::get().log("omfg acceptable packet sent");
			m_names.push_back(clientRequestedName);
			m_players.insert(std::pair<SOCKET, irr::core::stringw>(s, clientRequestedName));
			//m_waitingForApproval.erase(std::remove(m_waitingForApproval.begin(), m_waitingForApproval.end(), s), m_waitingForApproval.end());
			m_approvalRemoval.push_back(s);

			/*
			if(m_advertiser)
				m_advertiser->setNumberPlayers(m_names.size());
			*/
			if(m_view)
			{
				refreshNames();
			}
		}
		else
		{
			System::get().log("omfg couldn't sent acceptable packet");

			acceptable = false;
			//nope, not acceptable
			closesocket(s);
			//m_waitingForApproval.erase(std::remove(m_waitingForApproval.begin(), m_waitingForApproval.end(), s), m_waitingForApproval.end());
			m_approvalRemoval.push_back(s);
		}
		delete packet;
	}
}

void LANServer::receiveData()
{
	bool needToRefreshNames = false;
	for(std::map<SOCKET, irr::core::stringw>::const_iterator it = m_players.begin(); it != m_players.end();)
	{
		SOCKET s = it->first;
		char sizebuff[TCP_HEADER_SIZE];
		bool shouldIncrement = true;
		switch(TCPPacket::receiveHeader(s, sizebuff)){
			case NO_VALID_SOCKET:
				//do nothing because this shouldn't happen
				assert(0);
				break;
			case CAN_SKIP_CYCLE:
				//do nothing, this is like a Sleep()
				break;
			case RECEIVED_EOF:
			{
				NetworkController::get().getServerScene()->replaceAgent(getAgentID(s));

				System::get().log("closing connection, received EOF while trying to receive header");
				m_names.erase(std::remove(m_names.begin(), m_names.end(), it->second), m_names.end());
				closesocket(s);
				shouldIncrement = false;
				it = m_players.erase(it);
				needToRefreshNames = true;
				break;
			}
			case RECEIVE_ERROR:
			{
				NetworkController::get().getServerScene()->replaceAgent(getAgentID(s));

				core::stringc errorMsg("recv() failed with ");
				errorMsg+=getSocketError(WSAGetLastError());
				System::get().log(errorMsg.c_str());
				m_names.erase(std::remove(m_names.begin(), m_names.end(), it->second), m_names.end());
				closesocket(s);
				shouldIncrement = false;
				it = m_players.erase(it);
				needToRefreshNames = true;
				break;
			}
			case RECEIVED_HEADER_SUCCESSFULLY:
			{
				TCPPacket* p = TCPPacketFactory::get().createPacket(sizebuff);
				if(p)
				{
					core::stringc infoMsg(" m_players-> received packet successfully, type=");
					infoMsg+=p->getType();
					System::get().log(infoMsg.c_str());
					p->receive(s);
				}
				delete p;
				break;
			}
			case RECEIVED_WRONG_SIZE_HEADER:
				System::get().log("received weird header, ignoring.");
				break;
		}
		if(shouldIncrement)
		{
			++it;
		}
	}
	if(m_isInLobby && needToRefreshNames)
	{
		refreshNames();
	}

	for(std::vector<SOCKET>::const_iterator it = m_waitingForApproval.begin(); it != m_waitingForApproval.end();)
	{
		SOCKET s = *it;
		char sizebuff[TCP_HEADER_SIZE];
		bool shouldIncrement = true;
		int ret = TCPPacket::receiveHeader(s, sizebuff);
		switch(ret){
			case NO_VALID_SOCKET:
				//do nothing because this shouldn't happen
				break;
			case CAN_SKIP_CYCLE:
				//do nothing, this is like a Sleep()
				break;
			case RECEIVED_EOF:
			{
				System::get().log("closing connection, received EOF while trying to receive header");
				closesocket(s);
				shouldIncrement = false;
				it = m_waitingForApproval.erase(std::remove(m_waitingForApproval.begin(), m_waitingForApproval.end(), s), m_waitingForApproval.end());
				break;
			}
			case RECEIVE_ERROR:
			{
				core::stringc errorMsg("recv() failed with ");
				errorMsg+=getSocketError(WSAGetLastError());
				closesocket(s);
				shouldIncrement = false;
				it = m_waitingForApproval.erase(std::remove(m_waitingForApproval.begin(), m_waitingForApproval.end(), s), m_waitingForApproval.end());
				break;
			}
			case RECEIVED_HEADER_SUCCESSFULLY:
			{
				TCPPacket* p = TCPPacketFactory::get().createPacket(sizebuff);
				if(p)
				{
					core::stringc infoMsg(" m_waitingForApproval-> received packet successfully, type=");
					infoMsg+=p->getType();
					System::get().log(infoMsg.c_str());
					p->receive(s);
				}
				delete p;
				break;
			}
			case RECEIVED_WRONG_SIZE_HEADER:
				System::get().log("received weird header, ignoring.");
				break;
		}
		if(shouldIncrement)
		{
			++it;
		}
	}
}

void LANServer::sendPacket(TCPPacket* packet)
{
	for(std::map<SOCKET, irr::core::stringw>::const_iterator it = m_players.begin(); it != m_players.end(); ++it)
	{
		packet->send(it->first);
	}
}

void LANServer::setIsInLobby(const bool b)
{
	m_isInLobby = b;
}

void LANServer::setView(LANServerGUIView* view)
{
	m_view = view;
}

void LANServer::disconnectEveryone()
{
	m_lock.Lock();
	if(m_doDisconnectAll)
	{
		for(std::map<SOCKET, irr::core::stringw>::const_iterator it = m_players.begin(); it != m_players.end(); ++it)
		{
			shutdown(it->first, SD_BOTH);
			closesocket(it->first);
		}
		m_players.clear();
		m_names.clear();
		m_names.push_back(m_nickname);
	}
	m_lock.Unlock();
}

void LANServer::addShell(LockPointer<SpaceObjectShell>* shell)
{
	m_ShellListLock.Lock();
	m_Shells.push_back(shell);
	m_ShellListLock.Unlock(); 
}

unsigned int LANServer::getAvailableAgentID()
{
	return ++ID;
}

const wchar_t* LANServer::getServerNickname() const
{
	return m_nickname.c_str();
}

unsigned int LANServer::getTeamForName(const wchar_t* _name) const
{
	unsigned int cpt = 0;
	core::stringw name(_name);
	for(std::vector<core::stringw>::const_iterator it = m_names.begin(); it != m_names.end(); ++it, ++cpt)
	{
		core::stringw n = *it;
		if(n.equals_ignore_case(name))
			break;
	}
	return (cpt % 2 == 0) ? MASK_GROUP_1 : MASK_GROUP_2;
}

unsigned int LANServer::getTeamForSocket(SOCKET s) const
{
	std::map<SOCKET, irr::core::stringw>::const_iterator it = m_players.find(s);
	if(it != m_players.end())
	{
		return getTeamForName(it->second.c_str());
	}
	assert(0);//this shouldn't happen
	return 0;
}

unsigned int LANServer::getAgentID(SOCKET s) const
{
	std::map<SOCKET, unsigned int>::const_iterator it = m_playerAgents.find(s);
	if(it != m_playerAgents.end())
	{
		return it->second;
	}
	return 0;
}

const wchar_t* LANServer::getName(SOCKET s) const
{
	std::map<SOCKET, irr::core::stringw>::const_iterator it = m_players.find(s);
	if(it != m_players.end())
	{
		return it->second.c_str();
	}
	assert(0);//this shouldn't happen
	return L"<ERROR>";
}

void LANServer::initializeAllShips()
{
	System::get().log("Creating all ships");

	LANServerScene* scene = NetworkController::get().getServerScene();
	assert(scene);

	Agent* myAgent = scene->GetAgent();
	myAgent->SetID(getAvailableAgentID());
	unsigned int group = getTeamForName(getServerNickname());
	myAgent->SetMask(group | MASK_SHIP);
	myAgent->Init();

	
	for(std::map<SOCKET, unsigned int>::const_iterator it = m_playerAgents.begin(); it != m_playerAgents.end(); ++it)
	{
		SOCKET s = it->first;
		unsigned int theMask = MASK_SHIP | getTeamForSocket(s);
		core::vector3df pos = scene->GetNextSpawnPoint(theMask);
		SpaceObjectFactory::Get().CreateShip(scene, ACT_NET_SERVER, it->second, getName(s), theMask, pos, pos);//create the ship locally, which will add it to the server's shell list with the "NEEDS CREATION" flag set to true
	}
}

void LANServer::insertAgent(SOCKET s, const unsigned int agentID)
{
	m_playerAgents[s] = agentID;
}

void LANServer::refreshNames()
{
	if(m_advertiser)
		m_advertiser->setNumberPlayers(m_names.size());

	std::vector<core::stringw> teamA;
	std::vector<core::stringw> teamB;

	//TODO make use of the getTeamForName function so that the team repartition is done in a uniform way
	for(std::vector<core::stringw>::const_iterator it = m_names.begin(); it != m_names.end(); ++it)
	{
		core::stringw n = *it;
		if(teamA.size() > teamB.size())
			teamB.push_back(n);
		else
			teamA.push_back(n);
	}

	NameHelper helper;
	helper.teamA = teamA;
	helper.teamB = teamB;
	if(m_view)
	{
		m_view->updateNames(teamA, teamB);
		TCPPacket* packet = TCPPacketFactory::get().createPacket(REFRESH_NAME_LIST, &helper, 0);
		sendPacket(packet);
		delete packet;
	}
}

