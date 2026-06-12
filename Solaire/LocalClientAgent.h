#pragma once
#include "Agent.h"

class LocalClientAgent : public Agent
{
public:
	LocalClientAgent();
	~LocalClientAgent();

	void Init(); 
	void Update(float dt);

private:

	bool m_initialized;
};

