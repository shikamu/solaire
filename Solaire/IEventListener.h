#pragma once
#include <IEventReceiver.h>

class IEventListener
{
public:
	IEventListener();
	virtual ~IEventListener();

	virtual bool onKeyEvent(const irr::SEvent::SKeyInput& evt);
	virtual bool onMouseEvent(const irr::SEvent::SMouseInput& evt);
	virtual bool onJoystickEvent(const irr::SEvent::SJoystickEvent& evt);
	virtual bool onGUIEvent(const irr::SEvent::SGUIEvent& evt);
	virtual bool onLogEvent(const irr::SEvent::SLogEvent& evt);
	virtual bool onUserEvent(const irr::SEvent::SUserEvent& evt);
};

