#pragma once 

#include "Agent.h"

class DummyAgent : public Agent
{
private:

public:
	DummyAgent() {} 
	~DummyAgent() {} 
	void Init(); 
	void Update(float dt);
	
};