#pragma once

const unsigned int WCHAR_SIZE = sizeof(wchar_t);

enum TCPPacketType
{
	SEND_NAME,
	ACCEPT_NAME,
	REFRESH_NAME_LIST,
	SEND_CHAT_TEXT,

	CLIENT_ACTUATOR_DATA,

	CREATE_INITIAL_SHIPS_AND_AGENT,

	CREATE_SPACEOBJECT,
	UPDATE_SPACEOBJECT,
	DELETE_SPACEOBJECT,

	TOGGLE_REQUEST_SCORE_UPDATES,
	UPDATE_SCORE,

	TCPPACKETTYPE_FORCE_32_BIT = 0x7fffffff //This enum is never used, it only forces the compiler to compile these enumeration values to 32 bit.
};


//some custom return codes
static const unsigned int RECEIVED_HEADER_SUCCESSFULLY			= 0x00000008;
static const unsigned int RECEIVED_WRONG_SIZE_HEADER			= 0x00000009;
static const unsigned int NO_VALID_SOCKET						= 0x00000010;
static const unsigned int CAN_SKIP_CYCLE						= 0x00000020;
static const unsigned int RECEIVED_EOF							= 0x00000030;
static const unsigned int RECEIVE_ERROR							= 0x00000040;


//other TCP-related constants
static const unsigned int TCP_HEADER_SIZE = 8;//in bytes

//Upper bound on a single packet's total size (header + body). A size field read off
//the wire that is smaller than the header or larger than this is treated as a framing
//error / corruption and causes the connection to be dropped rather than trusting it for
//a heap allocation or buffer parse. 1 MB is far larger than any legitimate packet (the
//biggest is CREATE_INITIAL_SHIPS_AND_AGENT, a few KB even with many players).
static const unsigned int MAX_TCP_PACKET_SIZE = 1024 * 1024;