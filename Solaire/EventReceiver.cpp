#include <algorithm>
#include <irrlicht.h>

#include "EventReceiver.h"
#include "IEventListener.h"
#include "System.h"


using namespace irr;

EventReceiver::EventReceiver() : m_leftClick(false), m_rightClick(false), m_middleClick(false), m_joystickEnabled(false)
{
	for (u32 i=0; i< KEY_KEY_CODES_COUNT; ++i)
        m_keystate[i] = false;
}

EventReceiver::~EventReceiver()
{
}

void EventReceiver::reinit()
{
	m_joystickEnabled = false;

	if(System::get().getDevice()->activateJoysticks(m_joystickInfo))
	{

		m_joystickEnabled = m_joystickInfo.size() > 0;
	}
}

bool EventReceiver::OnEvent(const irr::SEvent& event)
{
	bool processed = false;
	switch(event.EventType)
	{
		case EET_KEY_INPUT_EVENT:
			m_keystate[event.KeyInput.Key] = event.KeyInput.PressedDown;
			for(m_it = m_listeners.begin(); m_it != m_listeners.end();)
			{
				processed |= (*m_it)->onKeyEvent(event.KeyInput);
				if(m_it != m_listeners.end())
					++m_it;
			}
			return processed;
		case EET_MOUSE_INPUT_EVENT:
			m_leftClick = event.MouseInput.isLeftPressed();
			m_rightClick = event.MouseInput.isRightPressed();
			m_middleClick = event.MouseInput.isMiddlePressed();
			for(m_it = m_listeners.begin(); m_it != m_listeners.end();)
			{
				processed |= (*m_it)->onMouseEvent(event.MouseInput);
				if(m_it != m_listeners.end())
					++m_it;
			}
			return processed;
		case EET_JOYSTICK_INPUT_EVENT:
		{
			m_joystickState = event.JoystickEvent;
			for(m_it = m_listeners.begin(); m_it != m_listeners.end();)
			{
				processed |= (*m_it)->onJoystickEvent(event.JoystickEvent);
				if(m_it != m_listeners.end())
					++m_it;
			}
			return processed;
		}
		case EET_GUI_EVENT:
		{
			for(m_it = m_listeners.begin(); m_it != m_listeners.end();)
			{
				processed |= (*m_it)->onGUIEvent(event.GUIEvent);
				if(m_it != m_listeners.end())
					++m_it;
			}
			return processed;
		}
		case EET_LOG_TEXT_EVENT:
			for(m_it = m_listeners.begin(); m_it != m_listeners.end();)
			{
				processed |= (*m_it)->onLogEvent(event.LogEvent);
				if(m_it != m_listeners.end())
					++m_it;
			}
			return processed;
		case EET_USER_EVENT:
			for(m_it = m_listeners.begin(); m_it != m_listeners.end();)
			{
				processed |= (*m_it)->onUserEvent(event.UserEvent);
				if(m_it != m_listeners.end())
					++m_it;
			}
			return processed;
		default:
			return false;
	}
}


void EventReceiver::registerEventListener(IEventListener* listener)
{
	m_listeners.push_back(listener);
}

void EventReceiver::unregisterEventListener(IEventListener* listener)
{
	m_it = m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
}

bool EventReceiver::isKeyDown(irr::EKEY_CODE keyCode) const
{
	if(keyCode < KEY_KEY_CODES_COUNT)
		return m_keystate[keyCode];
	return false;
}

bool EventReceiver::isLeftClickDown() const
{
	return m_leftClick;
}

bool EventReceiver::isRightClickDown() const
{
	return m_rightClick;
}

bool EventReceiver::isMiddleClickDown() const
{
	return m_middleClick;
}

bool EventReceiver::isJoystickEnabled() const
{
	return m_joystickEnabled;
}
