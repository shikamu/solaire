#pragma once
#include "NetworkConstants.h"

class TCPPacket;

class TCPPacketFactory{
public:

	static TCPPacketFactory& get();

	TCPPacket* createPacket(const TCPPacketType type, void* data, const unsigned int size) const;
	TCPPacket* createPacket(char header[TCP_HEADER_SIZE]) const;

private:
	TCPPacketFactory();
	~TCPPacketFactory();

	TCPPacket* instantiate(const TCPPacketType type) const;

	static TCPPacketFactory m_TCPPacketFactory;
};

