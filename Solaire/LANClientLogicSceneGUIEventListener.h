#pragma once
#include "LogicSceneGUIEventListener.h"

class LANClientLogicSceneGUIEventListener : public LogicSceneGUIEventListener
{
public:
	LANClientLogicSceneGUIEventListener();
	~LANClientLogicSceneGUIEventListener();

	bool onGUIEvent(const irr::SEvent::SGUIEvent& evt);
};

