#pragma once
#include "IEventListener.h"

class BasicLogEventListener : public IEventListener
{
public:
	BasicLogEventListener();
	~BasicLogEventListener();
	bool onLogEvent(const irr::SEvent::SLogEvent& evt);

};

