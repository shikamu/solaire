#pragma once
#include "Agent.h"


class LocalAgent : public Agent
{
private:

	bool m_needsInitiliazation;

public:
	LocalAgent() : m_needsInitiliazation(true) 
	{ 
		m_Name = L"NoName"; 
	}
	
	LocalAgent(const wchar_t* name, const bool needsInit); 

	~LocalAgent();

	void Init(); 
	void Update(float dt);
};