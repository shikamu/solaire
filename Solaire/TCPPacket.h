#pragma once
#include <winsock2.h>
#include "NetworkConstants.h"


class TCPPacket
{
	friend class TCPPacketFactory;

public:
	
	static int receiveHeader(SOCKET s, char dest[TCP_HEADER_SIZE]);

	TCPPacket();

	virtual ~TCPPacket();

	virtual bool init(void* data, const unsigned int);

	virtual TCPPacketType getType() const = 0;

	virtual bool send(SOCKET s);

	//The default implementation of this method assumes we've read a header yet and set the packet's size
	//pre-condition: we've read the header
	virtual bool receive(SOCKET s);

protected:

	unsigned int m_size;
	char* m_data;

	bool m_initialized;

};

class SendNamePacket : public TCPPacket
{
public:
	SendNamePacket();
	~SendNamePacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);

};

class AcceptNamePacket : public TCPPacket
{
public:
	AcceptNamePacket();
	~AcceptNamePacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);

};

class RefreshNamesPacket : public TCPPacket
{
public:
	RefreshNamesPacket();
	~RefreshNamesPacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);

};

class SendChatTextPacket : public TCPPacket
{
public:
	SendChatTextPacket();
	~SendChatTextPacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);
};

class UpdateSpaceObjectPacket : public TCPPacket
{
public:
	UpdateSpaceObjectPacket();
	~UpdateSpaceObjectPacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);

};

class ClientActuatorDataPacket : public TCPPacket
{
public:
	ClientActuatorDataPacket();
	~ClientActuatorDataPacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);

};

class CreateSpaceObjectPacket : public TCPPacket
{
public:
	CreateSpaceObjectPacket();
	~CreateSpaceObjectPacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);

};

class CreateInitialShipsAndAgentPacket : public TCPPacket
{
public:
	CreateInitialShipsAndAgentPacket();
	~CreateInitialShipsAndAgentPacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);

};

class DeleteSpaceObjectPacket : public TCPPacket
{
public:
	DeleteSpaceObjectPacket();
	~DeleteSpaceObjectPacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);
};

class UpdateScorePacket : public TCPPacket
{
public:
	UpdateScorePacket();
	~UpdateScorePacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);

};

class ToggleRequestScorePacket : public TCPPacket
{
public:
	ToggleRequestScorePacket();
	~ToggleRequestScorePacket();

	TCPPacketType getType() const;

	bool init(void* data, const unsigned int);

	bool send(SOCKET s);
	bool receive(SOCKET s);
};

