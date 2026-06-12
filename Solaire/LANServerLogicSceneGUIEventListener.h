#pragma once
#include "LogicSceneGUIEventListener.h"

class LANServerLogicSceneGUIEventListener : public LogicSceneGUIEventListener
{
public:
	LANServerLogicSceneGUIEventListener();
	~LANServerLogicSceneGUIEventListener();

	bool onGUIEvent(const irr::SEvent::SGUIEvent& evt);
};

