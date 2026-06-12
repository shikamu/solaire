#pragma once
#include <vector>
#include <WinSock2.h>

class SpaceObjectShell;

struct LANCreationAssociationHelper
{

	LANCreationAssociationHelper() : agentID(0), shipID(0), mask(0){}
	
	unsigned int agentID;
	unsigned int mask;
	unsigned int shipID;
};

struct LANCreationHelper
{
	//the content of this structure will be received by a client

	LANCreationHelper() : agentID(0), shipID(0), dirty(false){}

	std::vector<SpaceObjectShell*> allShips;	//this vector contains all the information needed to create the initial ships

	unsigned int agentID;	//this is the id that the local agent of the client should be
	unsigned int shipID;	//this is the id of the ship that the client will control
	unsigned int mask;		//this is the mask for the client, it's only useful for the team since the server is sorting them

	bool dirty;	//this tells whether this object has been consumed yet. this is for convenience to know whether or not we should delete the pointer that holds this structure in the client scene
};
