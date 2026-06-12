#pragma once

class LANServer;
class LANFinalView;
class LANClient;
class TCPPacket;
class LANServerScene;

class NetworkController
{
public:
	static NetworkController& get();

	void registerLANServer(LANServer* server);
	void unregisterLANServer();

	void registerLANClient(LANClient* client);
	void unregisterLANClient();

	void registerFinalLANView(LANFinalView* view);
	void unregisterFinalLANView();

	void registerServerScene(LANServerScene* scene);
	

	LANServer* getServer() const;

	LANClient* getClient() const;

	LANFinalView* getLANFinalView() const;

	LANServerScene* getServerScene() const;

	void sendPacket(TCPPacket* packet);

private:
	
	enum NETWORKING_STATE{
		NETWORKING_IDLING = 1,
		
	};
	
	NetworkController();
	~NetworkController();

	static NetworkController m_NetworkController;

	bool doWSAStartup();

	void reset();

	LANServer* m_server;
	LANFinalView* m_LANFinalview;
	LANClient* m_client;
	LANServerScene* m_serverScene;

	NETWORKING_STATE m_state;

	bool m_successfulWSAStartup;

};

