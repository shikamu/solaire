#include <cassert>
#include <vector>
#include <irrlicht.h>

#include "KeybindingListener.h"
#include "System.h"
#include "ConfigData.h"
#include "GUIConstants.h"
#include "OptionsGUIView.h"
#include "Utilities.h"
#include "KeyBindings.h"
#include "InputConstants.h"

using namespace irr;

KeybindingListener::KeybindingListener(OptionsGUIView* options) : m_options(options), m_lastMouseX(0), m_lastMouseY(0)
{
}

KeybindingListener::~KeybindingListener()
{
}

bool KeybindingListener::onKeyEvent(const irr::SEvent::SKeyInput& evt)
{
	if(evt.Key == KEY_ESCAPE)
	{
		gui::IGUITable* table = getTablePointer();
		if(table)
		{
			s32 col = table->getActiveColumn();
			s32 row = table->getSelected();
			if(isCellEditable(row, col))
			{
				Binding* binding = (Binding*)table->getCellData(row, col);
				if(binding)
				{
					if(binding->getCode() == INPUT_NONE)
					{
						table->setCellText(row, col, Utilities::getPrintable(INPUT_NONE), video::SColor(255, 255, 255, 255));
					}
				}
			}
		}
		m_options->setKeybindingTableLocked(false);
	}
	else if(m_options->isKeybindingTableLocked())
	{
		gui::IGUITable* table = getTablePointer();
		if(table)
		{
			s32 col = table->getActiveColumn();
			s32 row = table->getSelected();
			if(isCellEditable(row, col))
			{
				Binding* binding = verifyBinding(table, BINDING_KEYBOARD, row, col);
				if(!binding)
				{
					System::get().log("warning: KeybindingListener::onKeyEvent binding pointer is NULL");
					return false;
				}

				if(canInsert(Utilities::getPrintable((SolaireInputCode)evt.Key), binding))
				{
					binding->push((SolaireInputCode)evt.Key);
					table->setCellText(row, col, binding->toString(), video::SColor(255, 255, 255, 255));
				}
			}
		}
	}
	return false;
}

bool KeybindingListener::onMouseEvent(const irr::SEvent::SMouseInput& evt)
{
	gui::IGUITable* table = getTablePointer();
	if(table)
	{
		core::rect<s32> r = table->getAbsoluteClippingRect();
		if(r.isPointInside(core::vector2d<s32>(evt.X, evt.Y)))
		{
			if(evt.X < r.UpperLeftCorner.X + r.getWidth()*0.33f)
			{
				table->setActiveColumn(0);				
			}
			else if(evt.X < r.UpperLeftCorner.X + r.getWidth()*0.66f)
			{
				table->setActiveColumn(1);
			}
			else if(evt.X < r.UpperLeftCorner.X + r.getWidth())
			{
				table->setActiveColumn(2);
			}
		}

		s32 col = table->getActiveColumn();
		s32 row = table->getSelected();

		if(isCellEditable(row, col))
		{
			if(m_options->isKeybindingTableLocked())
			{
				Binding* binding = verifyBinding(table, BINDING_MOUSE, row, col);

				if(!binding)
				{
					System::get().log("warning: KeybindingListener::onMouseEvent binding pointer is NULL");
					return false;
				}

				if(evt.Event == EMIE_MOUSE_WHEEL)
				{
					f32 wheel = evt.Wheel;
					if(wheel > 0.0f)
					{
						binding->push(MOUSE_WHEEL_UP);
					}
					else
					{
						binding->push(MOUSE_WHEEL_DOWN);
					}
					table->setCellText(row, col, binding->toString(), video::SColor(255, 255, 255, 255));
					m_options->setKeybindingTableLocked(false);
					return true;//prevents the table from getting scrolling events
				}else
				{
					bool somethingHappened = false;
					if(evt.isLeftPressed())
					{
						binding->push(MOUSE_LEFT_CLICK);
						somethingHappened = true;
					}
					else if(evt.isRightPressed())
					{
						binding->push(MOUSE_RIGHT_CLICK);
						somethingHappened = true;
					}
					else if(evt.isMiddlePressed())
					{
						binding->push(MOUSE_MIDDLE_CLICK);
						somethingHappened = true;
					}
					else
					{
						if(evt.Event == EMIE_MOUSE_MOVED)
						{
							s32 deltaX = evt.X - m_lastMouseX;
							s32 deltaY = evt.Y - m_lastMouseY;
							const s32 sensitivity = 6;
							m_lastMouseX = evt.X;
							m_lastMouseY = evt.Y;
							if(abs(deltaX) > abs(deltaY))
							{
								if(deltaX > sensitivity)
								{
									binding->push(MOUSE_MOVE_RIGHT);
									somethingHappened = true;
								}
								else if(deltaX < -sensitivity)
								{
									binding->push(MOUSE_MOVE_LEFT);
									somethingHappened = true;
								}
							}
							else
							{
								if(deltaY > sensitivity)
								{
									binding->push(MOUSE_MOVE_DOWN);
									somethingHappened = true;
								}
								else if(deltaY < -sensitivity)
								{
									binding->push(MOUSE_MOVE_UP);
									somethingHappened = true;
								}
							}
						}
					}
					if(somethingHappened)
					{
						table->setCellText(row, col, binding->toString(), video::SColor(255, 255, 255, 255));
						m_options->setKeybindingTableLocked(false);
						return true;//prevents the table from getting scrolling events
					}
				}
			}
			else
			{
				if(evt.Event == EMIE_LMOUSE_DOUBLE_CLICK)
				{
					m_lastMouseX = evt.X;
					m_lastMouseY = evt.Y;
					m_options->setKeybindingTableLocked(true);
					//table->setCellText(row, col, Utilities::getPrintable(INPUT_NONE), video::SColor(255, 255, 255, 255));
					table->setCellText(row, col, L"", video::SColor(255, 255, 255, 255));
				}
			}
		}
	}

	return false;
}

bool KeybindingListener::onJoystickEvent(const SEvent::SJoystickEvent& evt)
{
	if(m_options->isKeybindingTableLocked())
	{
		gui::IGUITable* table = getTablePointer();
		if(table)
		{
			s32 col = table->getActiveColumn();
			s32 row = table->getSelected();
			if(isCellEditable(row, col))
			{
				u32 buttons = evt.ButtonStates;
				unsigned int axis = SEvent::SJoystickEvent::NUMBER_OF_AXES;
				//take the first axis that has the an absolute value above 20000, if any
				//TODO possibly make the difference between evt.Axis[i] being positive or negative, that way users could reverse our internal pitching/rolling/yawing logic if they want
				for(unsigned int i = 0; i < SEvent::SJoystickEvent::NUMBER_OF_AXES; ++i)
				{
					if(abs(evt.Axis[i]) > 25000.0f)
					{
						axis = i;


						Binding* binding = verifyBinding(table, BINDING_JOYSTICK, row, col);
						if(!binding)
						{
							System::get().log("warning: KeybindingListener::onJoystickEvent binding pointer is NULL");
							return false;
						}

						//if an axis was activated
						binding->clear();
						binding->push((SolaireInputCode)(JOYSTICK_AXIS_START + 1 + axis));
						table->setCellText(row, col, binding->toString(), video::SColor(255, 255, 255, 255));
						m_options->setKeybindingTableLocked(false);
						return true;

						//break;
					}
				}

				if(axis >= SEvent::SJoystickEvent::NUMBER_OF_AXES)
				{
					if(buttons > 0)
					{

						Binding* binding = verifyBinding(table, BINDING_JOYSTICK, row, col);
						if(!binding)
						{
							System::get().log("warning: KeybindingListener::onJoystickEvent binding pointer is NULL");
							return false;
						}
						//looks like we didn't pick an axis for this, let's see if there's a button
						bool hasInserted = false;
						for(unsigned int i = 0; i < JOYSTICK_BUTTON_END - JOYSTICK_BUTTON_START - 1; ++i)
						{
							if(evt.IsButtonPressed(i))
							{

								if(canInsert(Utilities::getPrintable((SolaireInputCode)(JOYSTICK_BUTTON_START + 1 + i)), binding))
								{
									hasInserted = true;
									binding->push((SolaireInputCode)(JOYSTICK_BUTTON_START + 1 + i));
									table->setCellText(row, col, binding->toString(), video::SColor(255, 255, 255, 255));
								}
							}
						}
						if(hasInserted)
						{
							return true;
						}
					}
				}
			}
		}
	}

	
	return false;
}

irr::gui::IGUITable* KeybindingListener::getTablePointer() const
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	gui::IGUIElement* elt = env->getRootGUIElement()->getElementFromId(GUI_ID_OPTIONSMENU_KEYBINDINGS_TABLE, true);

	//can't dynamic_cast, so has to use hasType(..) and then standard casting
	if(elt && elt->hasType(gui::EGUIET_TABLE) && elt->isVisible())
	{
		gui::IGUITable* table = (gui::IGUITable*)elt;		
		return table;
	}
	return NULL;
}

bool KeybindingListener::isCellEditable(const irr::s32 row, const irr::s32 col) const
{
	//no editing under the "action" column and the last row is a dummy row
	gui::IGUITable* table = getTablePointer();
	if(table)
		return col > 0 && row >= 0 && row < table->getRowCount()-1 && !(col == 1 && row == 0);
	return false;
}

bool KeybindingListener::isCellEditable(irr::gui::IGUITable* table) const
{
	return isCellEditable(table->getSelected(), table->getActiveColumn());
}

Binding* KeybindingListener::verifyBinding(gui::IGUITable* table, const BindingType type, const s32 row, const s32 col) const
{
	Binding* binding = (Binding*)table->getCellData(row, col);
	if(binding)
	{
		if(binding->getType() != type)
		{
			Action a = System::get().getConfig()->getKeybindings().getActionFor(binding);
			if(a > NOT_AN_ACTION_FIRST && a < NOT_AN_ACTION_COUNT)
			{
				binding = System::get().getConfig()->getKeybindings().rebindEmpty(a, type, col == 1);
				table->setCellData(row, col, binding);				
			}
		}
		return binding;
	}
	else
	{
		assert(0); //shouldn't happen
	}
	return NULL;
}

bool KeybindingListener::canInsert(const core::stringw& incoming, Binding* binding)
{
	if(!binding)
		return false;

	//core::stringw incoming(Utilities::getPrintable((SolaireInputCode)(JOYSTICK_BUTTON_START + 1 + i)));
	core::stringw current(binding->toString());
	std::vector<core::stringw> elements;
	current.split(elements, L"+");
	bool can = true;
	for(std::vector<core::stringw>::const_iterator it = elements.begin(); it != elements.end(); ++it)
	{
		core::stringw s = *it;
		if(s.equals_ignore_case(incoming))
		{
			can = false;
			break;
		}
	}
	return can;
}

