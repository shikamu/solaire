#include "IEventListener.h"

using namespace irr;

IEventListener::IEventListener()
{
}


IEventListener::~IEventListener()
{
}

bool IEventListener::onKeyEvent(const irr::SEvent::SKeyInput& evt)
{
	return false;
}

bool IEventListener::onMouseEvent(const irr::SEvent::SMouseInput& evt)
{
	return false;
}

bool IEventListener::onJoystickEvent(const irr::SEvent::SJoystickEvent& evt)
{
	return false;
}

bool IEventListener::onGUIEvent(const irr::SEvent::SGUIEvent& evt)
{
	return false;
}

bool IEventListener::onLogEvent(const irr::SEvent::SLogEvent& evt)
{
	return false;
}

bool IEventListener::onUserEvent(const irr::SEvent::SUserEvent& evt)
{
	return false;
}
