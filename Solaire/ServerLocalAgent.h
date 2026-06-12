#pragma once
#include "LocalAgent.h"


class ServerLocalAgent : public LocalAgent
{
private:
public:
	ServerLocalAgent(const wchar_t* name);

	~ServerLocalAgent() 
	{
	} 
 
};