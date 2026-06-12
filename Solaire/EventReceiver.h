#pragma once
/*
#define NOMINMAX
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
*/

#include <list>
#include <IEventReceiver.h>

#include <irrArray.h>

class IEventListener;

class EventReceiver : public irr::IEventReceiver
{
public:
	EventReceiver();
	~EventReceiver();

	void reinit();

	bool OnEvent(const irr::SEvent& event);

	void registerEventListener(IEventListener* listener);
	void unregisterEventListener(IEventListener* listener);

	bool isKeyDown(irr::EKEY_CODE keyCode) const;

	const irr::SEvent::SJoystickEvent& getJoystickState() const
	{
		return m_joystickState;
	}

	bool isLeftClickDown() const;

	bool isRightClickDown() const;

	bool isMiddleClickDown() const;

	bool isJoystickEnabled() const;

private:

	irr::core::array<irr::SJoystickInfo> m_joystickInfo;

	std::list<IEventListener*>::const_iterator m_it;

	std::list<IEventListener*> m_listeners;

	bool m_keystate[irr::KEY_KEY_CODES_COUNT];

	irr::SEvent::SJoystickEvent m_joystickState;

	bool m_leftClick, m_rightClick, m_middleClick, m_joystickEnabled;

};

