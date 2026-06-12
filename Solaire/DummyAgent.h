#pragma once 

#include "Agent.h"

class DummyAgent : public Agent
{
private:
	bool m_advanced;
public:
	DummyAgent() : m_advanced(false) {}
	~DummyAgent() {}
	void Init();
	void Update(float dt);
	void SetAdvanced(const bool b) { m_advanced = b; }

};