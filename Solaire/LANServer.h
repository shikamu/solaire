#pragma once
#include <vector>
#include <map>
#include <winsock2.h>

#include <irrString.h>

#include "Task.h"
#include "CSLock.h"

class GameAdvertiser;
class LANServerGUIView;
class TCPPacket;

template<class T> class LockPointer;
class SpaceObjectShell;

class LANServer : public Task
{
	friend class LANServerScene;

public:
	LANServer(const wchar_t* playerName, GameAdvertiser* advertiser, LANServerGUIView* view);
	~LANServer();

	bool setup();

	void setPaused(const bool b);

	void checkName(SOCKET s, const char* name);

	//this function does not delete the packet
	void sendPacket(TCPPacket* packet);

	void setIsInLobby(const bool b);

	void setView(LANServerGUIView* view);

	void disconnectEveryone();

	void setDisconnectAll(const bool b)
	{
		m_doDisconnectAll = b;
	}

	void addShell(LockPointer<SpaceObjectShell>* shell);	

	//TODO use a system similar to what we're doing for the spaceobject's IDs (this is currently using ++ID as return value)
	unsigned int getAvailableAgentID();

	const wchar_t* getServerNickname() const;

	unsigned int getTeamForName(const wchar_t* name) const;

	unsigned int getTeamForSocket(SOCKET s) const;

	//returns 0 if no agents found
	unsigned int getAgentID(SOCKET s) const;

	const wchar_t* getName(SOCKET s) const;

	void initializeAllShips();

	void insertAgent(SOCKET s, const unsigned int agentID);

	void AcquireLock();
	void ReleaseLock();

	void CleanupShells();

	void toggleWantScore(SOCKET s);

	//Lobby ready-up. setPlayerReady is called when a client's PLAYER_READY packet arrives;
	//allPlayersReady (called from the main thread on PLAY) reports whether every connected
	//client has readied up.
	void setPlayerReady(SOCKET s, const bool ready);
	bool allPlayersReady();

	//Number of AI bots the host wants in the game (set from the lobby, read at game start).
	void setBotCount(const unsigned int n);
	unsigned int getBotCount() const;

	//Whether bots should use the advanced (harder) AI.
	void setBotsAdvanced(const bool b);
	bool getBotsAdvanced() const;

	//Sends a notification line to all clients and shows it on the host (lobby chat). Safe to
	//call from the main thread or the network thread.
	void broadcastSystemMessage(const wchar_t* text);

	//Sends an in-game notification line (kill feed / win banner) to all clients. The host shows
	//its own copy separately. Safe from the main thread or the network thread.
	void broadcastKillFeed(const wchar_t* text);

protected:
	unsigned int __stdcall run();

private:

	void showSetupErrorMessageBox(const wchar_t* msg);

	const std::map<SOCKET, irr::core::stringw> getPlayers() const
	{
		return m_players;
	}

	const std::vector<LockPointer<SpaceObjectShell>*>& getAllShells() const
	{
		return m_Shells;
	}

	static unsigned int ID;

	void markForApprovalRemoval(SOCKET s);

	void checkConnections();

	void checkNames();

	void receiveData();

	//Broadcasts a "<name> left the game" notification. Called when a client connection drops.
	void announcePlayerLeft(const irr::core::stringw& name);

	void refreshNames();

	std::vector<irr::core::stringw> m_names;
	std::vector<LockPointer<SpaceObjectShell>*> m_Shells;

	std::vector<SOCKET> m_approvalRemoval;
	std::vector<SOCKET> m_waitingForApproval;

	std::map<SOCKET, bool> m_wantScores;

	std::map<SOCKET, bool> m_ready;//lobby ready state per connected client socket

	std::map<SOCKET, irr::core::stringw> m_players;

	std::map<SOCKET, unsigned int> m_playerAgents; //agent ID for SOCKET, this is the list of all agents even those that haven't been confirmed
	irr::core::stringw m_nickname;

	GameAdvertiser* m_advertiser;
	LANServerGUIView* m_view;

	CSLock m_lock;
	CSLock m_ShellListLock;

	SOCKET m_serverSocket;
	unsigned int m_serverId;

	bool m_initialized, m_paused, m_isInLobby;

	bool m_doDisconnectAll;//this flag is used to bypass the disconnectEveryone call when the server view goes hidden. in all cases except one, we do indeed want to disconnect all, but when we press "play", this view gets hidden and we really don't want to disconnect everyone then

	unsigned int m_botCount;//number of AI bots to spawn when the game starts
	bool m_botsAdvanced;//whether those bots use the advanced AI

	LANServer(const LANServer& other);
	LANServer& operator=(const LANServer& other);
};

