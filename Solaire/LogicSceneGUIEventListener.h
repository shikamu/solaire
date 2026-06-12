#pragma once
#include "IEventListener.h"

class LogicSceneGUIEventListener : public IEventListener
{
public:
	LogicSceneGUIEventListener();
	~LogicSceneGUIEventListener();

	bool onGUIEvent(const irr::SEvent::SGUIEvent& evt);
};

