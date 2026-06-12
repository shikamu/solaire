#pragma once
#include "IEventListener.h"

class OptionsGUIView;
class Binding;
enum BindingType;

namespace irr
{
	namespace gui
	{
		class IGUITable;
	}
}

class KeybindingListener : public IEventListener
{
public:
	KeybindingListener(OptionsGUIView* options);
	~KeybindingListener();

	//allowed keybindings are: any combination of keys that are listed in InputConstants.h between KEYBOARD_FIRST and KEYBOARD_LAST. note that the same cannot appear multiple times for the same binding.
	bool onKeyEvent(const irr::SEvent::SKeyInput& evt);
	
	//allowed bindings are: any one key that are listed in InputConstants.h between MOUSE_FIRST	and MOUSE_LAST
	bool onMouseEvent(const irr::SEvent::SMouseInput& evt);
	
	//allowed bindings are one of:
	//1) any one of the axies that are listed in InputConstants.h between JOYSTICK_AXIS_START and JOYSTICK_AXIS_END
	//2) any combinations of buttons that are listed in InputConstants.h between JOYSTICK_BUTTON_START and JOYSTICK_BUTTON_END. note that the same button cannot appear multiple times for the same binding.
	bool onJoystickEvent(const irr::SEvent::SJoystickEvent& evt);



private:

	irr::gui::IGUITable* getTablePointer() const;
	bool isCellEditable(const irr::s32 row, const irr::s32 col) const;
	bool isCellEditable(irr::gui::IGUITable* table) const;
	Binding* verifyBinding(irr::gui::IGUITable* table, const BindingType type, const irr::s32 row, const irr::s32 col) const;

	bool canInsert(const irr::core::stringw& incoming, Binding* binding);

	OptionsGUIView* m_options;

	irr::s32 m_lastMouseX, m_lastMouseY;
};

