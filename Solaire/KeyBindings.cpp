#include <cassert>
#include <irrlicht.h>
#include <irrXML.h>

#include "KeyBindings.h"

#include "System.h"
#include "ConfigData.h"
#include "EventReceiver.h"
#include "Utilities.h"
#include "InputConstants.h"

using namespace irr;

static const float ONE_OVER_32767 = 1.0f / 32767.0f;

Binding::~Binding()
{

}

bool Binding::isTriggered() const
{
	return false;
}

float Binding::getValue() const
{
	return 0;
}

BindingType Binding::getType() const
{
	return BINDING_NOTYPE;
}

void Binding::writeToXML(io::IXMLWriter*) const
{

}

core::stringw Binding::toString() const
{
	return core::stringw(Utilities::getPrintable(INPUT_NONE));
}

void Binding::push(const SolaireInputCode key)
{

}

void Binding::clear()
{

}

SolaireInputCode Binding::getCode() const
{
	return INPUT_NONE;
}

bool Binding::isAxis() const
{
	return false;
}
	
void Binding::update(const float dt)
{

}

KeyboardBinding::KeyboardBinding(const std::vector<SolaireInputCode>& keys) : m_Value(0.0f)
{
	m_keys = keys;
}

KeyboardBinding::KeyboardBinding(const SolaireInputCode key) : m_Value(0.0f)
{
	m_keys.push_back(key);
}

KeyboardBinding::~KeyboardBinding()
{

}

bool KeyboardBinding::isTriggered() const
{
	bool ret = true;
	for(std::vector<SolaireInputCode>::const_iterator it = m_keys.begin(); ret && it != m_keys.end(); ++it)
	{
		ret &= System::get().getConfig()->getEventReceiver().isKeyDown((EKEY_CODE)(*it));
	}
	return ret;
}

float KeyboardBinding::getValue() const
{
	return m_Value; 
}

BindingType KeyboardBinding::getType() const
{
	return BINDING_KEYBOARD;
}

void KeyboardBinding::writeToXML(io::IXMLWriter* xml) const
{
	if(xml)
		for(std::vector<SolaireInputCode>::const_iterator it = m_keys.begin(); it != m_keys.end(); ++it)
		{
			core::stringw str(*it);
			xml->writeElement(L"key", true, L"code", str.c_str());
			xml->writeLineBreak();
		}
}

core::stringw KeyboardBinding::toString() const
{
	core::stringw str;
	if(m_keys.size() == 0)
		return Utilities::getPrintable(INPUT_NONE);

	for(std::vector<SolaireInputCode>::const_iterator it = m_keys.begin(); it != m_keys.end(); ++it)
	{
		if(str.equals_ignore_case("") || str.equals_ignore_case(Utilities::getPrintable(INPUT_NONE)))
			str = Utilities::getPrintable(*it);
		else
		{
			str += "+";
			str += Utilities::getPrintable(*it);
		}
	}
	return str;
}

void KeyboardBinding::push(const SolaireInputCode key)
{
	m_keys.push_back(key);
}

void KeyboardBinding::clear()
{
	m_keys.clear();
}

void KeyboardBinding::update(const float dt)
{
	float Modifier = 1.0f;

	if (isTriggered())
	{	
		m_Value = std::min(1.0f, m_Value + dt * 2.0f);
	}
	else
	{
		m_Value = std::max(0.0f, m_Value - dt * 3.0f);
	}
}

SolaireInputCode KeyboardBinding::getCode() const
{
	if(m_keys.size() > 0)
		return *m_keys.begin();
	return INPUT_NONE;
}

MouseBinding::MouseBinding(SolaireInputCode code) : m_code(code), m_dX(0.0f), m_dY(0.0f), m_IsReady(false)
{
	m_ControlX = System::get().getDevice()->getVideoDriver()->getScreenSize().Width / 2.0f;
	m_ControlY = System::get().getDevice()->getVideoDriver()->getScreenSize().Height / 2.0f;
	m_PosX = m_ControlX;
	m_PosY = m_ControlY;
}

bool MouseBinding::isTriggered() const
{
	
	switch(m_code)
	{
		case MOUSE_LEFT_CLICK:
			return System::get().getConfig()->getEventReceiver().isLeftClickDown();
		case MOUSE_RIGHT_CLICK:
			return System::get().getConfig()->getEventReceiver().isRightClickDown();
		case MOUSE_MIDDLE_CLICK:
			return System::get().getConfig()->getEventReceiver().isMiddleClickDown();
		case MOUSE_WHEEL_DOWN:
			return (System::get().getConfig()->getKeybindings().getMouseWheelValue()) < -0.5f;
		case MOUSE_WHEEL_UP:
			return (System::get().getConfig()->getKeybindings().getMouseWheelValue()) > 0.5f;
		case MOUSE_MOVE_DOWN:
		case MOUSE_MOVE_UP:
		case MOUSE_MOVE_LEFT:
		case MOUSE_MOVE_RIGHT:
			return true; 
		default:
			return false;
	}
	return false;
}

float MouseBinding::getValue() const
{
	switch(m_code)
	{
		default:
		case MOUSE_LEFT_CLICK:
		case MOUSE_RIGHT_CLICK:
		case MOUSE_MIDDLE_CLICK:
			return 0.0f;

		case MOUSE_WHEEL_DOWN:
		case MOUSE_WHEEL_UP:
			return System::get().getConfig()->getKeybindings().getMouseWheelValue();
		case MOUSE_MOVE_DOWN:
			return m_dY;
		case MOUSE_MOVE_UP:
			return -m_dY;
		case MOUSE_MOVE_LEFT:
			return m_dX;
		case MOUSE_MOVE_RIGHT:
			return -m_dX;
	}
	return 0.0f;
}

BindingType MouseBinding::getType() const
{
	return BINDING_MOUSE;
}

void MouseBinding::writeToXML(io::IXMLWriter* xml) const
{
	if(xml)
	{
		core::stringw str(m_code);
		xml->writeElement(L"mouse", true, L"code", str.c_str());
		xml->writeLineBreak();
	}
}

core::stringw MouseBinding::toString() const
{
	return core::stringw(Utilities::getPrintable(m_code));
}

void MouseBinding::push(const SolaireInputCode key)
{
	m_code = key;
}

void MouseBinding::clear()
{
	m_code = INPUT_NONE;
}

SolaireInputCode MouseBinding::getCode() const
{
	return m_code;		
}

void MouseBinding::update(const float dt)
{
	if (!m_IsReady)
	{
		m_PosX = m_ControlX;
		m_PosY = m_ControlY;
		m_IsReady = true;
		return; 
	}

	m_PosX = System::get().getConfig()->getKeybindings().getMousePosX();
	m_PosY = System::get().getConfig()->getKeybindings().getMousePosY();

	// Last number in function represents mouse sensitivity

	m_dX = std::max(-1.0f, std::min(1.0f, (m_PosX - m_ControlX) / m_ControlX * 3.0f));
	m_dY = std::max(-1.0f, std::min(1.0f, (m_PosY - m_ControlY) / m_ControlY * 3.0f));
}

JoysticBinding::JoysticBinding(const SolaireInputCode key) : KeyboardBinding(key)
{

}

JoysticBinding::JoysticBinding(const std::vector<SolaireInputCode>& keys) : KeyboardBinding(keys)
{

}

bool JoysticBinding::isTriggered() const
{
	if(!System::get().getConfig()->getEventReceiver().isJoystickEnabled())
		return false;

	bool ret = true;
	if(m_keys.size() > 1)
	{
		//buttons
		for(std::vector<SolaireInputCode>::const_iterator it = m_keys.begin(); ret && it != m_keys.end(); ++it)
		{
			SolaireInputCode code = *it;
			ret &= System::get().getConfig()->getEventReceiver().getJoystickState().IsButtonPressed(code - (JOYSTICK_BUTTON_START+1));
		}
		return ret;
	}
	else if(m_keys.size() == 1)
	{
		//it might still be a button
		SolaireInputCode code = *m_keys.begin();
		if(code > JOYSTICK_BUTTON_START && code < JOYSTICK_BUTTON_END)
		{
			return System::get().getConfig()->getEventReceiver().getJoystickState().IsButtonPressed(code - (JOYSTICK_BUTTON_START+1));
		}
		else if(code > JOYSTICK_AXIS_START && code < JOYSTICK_AXIS_END)
		{
			return abs(deadZoneCompensate(ONE_OVER_32767 * System::get().getConfig()->getEventReceiver().getJoystickState().Axis[code - (JOYSTICK_AXIS_START+1)])) > 0.0f;
		}
	}
	return false;
}

float JoysticBinding::getValue() const
{	
	if(!System::get().getConfig()->getEventReceiver().isJoystickEnabled())
		return 0.0f;

	if(m_keys.size() > 1)
	{
		//if it's buttons, use the same logic as for the keyboard
		return KeyboardBinding::getValue();
	}
	else if(m_keys.size() == 1)
	{
		SolaireInputCode code = *m_keys.begin();
		if(code > JOYSTICK_BUTTON_START && code < JOYSTICK_BUTTON_END)
		{
			return KeyboardBinding::getValue();
		}
		else if(code > JOYSTICK_AXIS_START && code < JOYSTICK_AXIS_END)
		{
			//return ONE_OVER_32767 * System::get().getConfig()->getEventReceiver().getJoystickState().Axis[code - (JOYSTICK_AXIS_START+1)];
			return deadZoneCompensate(ONE_OVER_32767 * System::get().getConfig()->getEventReceiver().getJoystickState().Axis[code - (JOYSTICK_AXIS_START+1)]);
		}
	}
	return 0.0f;
}

BindingType JoysticBinding::getType() const
{
	return BINDING_JOYSTICK;
}

void JoysticBinding::writeToXML(irr::io::IXMLWriter* xml) const
{
	if(xml)
		for(std::vector<SolaireInputCode>::const_iterator it = m_keys.begin(); it != m_keys.end(); ++it)
		{
			core::stringw str(*it);
			xml->writeElement(L"joystick", true, L"code", str.c_str());
			xml->writeLineBreak();
		}

}

bool JoysticBinding::isAxis() const
{
	if(m_keys.size() == 1)
	{
		SolaireInputCode code = *m_keys.begin();
		return code > JOYSTICK_AXIS_START && code < JOYSTICK_AXIS_END;
	}
	return false;
}

float JoysticBinding::deadZoneCompensate(const float Value, const float deadzone) const
{
	if (fabs(Value) < deadzone)
	{
		return 0.0f;
	}
	else
	{
		float Range = 1.0f - deadzone;		
		int Sign = static_cast<int>(fabs(Value) / Value);
		return (Value - deadzone*Sign) / Range;  
	}
}

KeyBindings::KeyBindings() : m_mouseDeltaX(0.0f), m_mouseDeltaY(0.0f), m_mouseWheel(0.0f), m_lastMouseX(0), m_lastMouseY(0)
{
	for(unsigned int i = NOT_AN_ACTION_FIRST + 1; i < NOT_AN_ACTION_COUNT; ++i)
	{
		m_primaryBindings.insert(std::pair<Action, Binding*>((Action)i, new Binding));
		m_secondaryBindings.insert(std::pair<Action, Binding*>((Action)i, new Binding));
	}
	/*
	m_primaryBindings.insert(std::pair<Action, Binding*>(GAME_MENU, new KeyboardBinding(KEYBOARD_KEY_ESCAPE)));
	m_primaryBindings.insert(std::pair<Action, Binding*>(PITCH_MORE, new KeyboardBinding(KEYBOARD_KEY_W)));
	m_primaryBindings.insert(std::pair<Action, Binding*>(PITCH_LESS, new KeyboardBinding(KEYBOARD_KEY_S)));
	m_primaryBindings.insert(std::pair<Action, Binding*>(YAW_MORE, new KeyboardBinding(KEYBOARD_KEY_A)));
	m_primaryBindings.insert(std::pair<Action, Binding*>(YAW_LESS, new KeyboardBinding(KEYBOARD_KEY_D)));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ROLL_MORE, new KeyboardBinding(KEYBOARD_KEY_Q)));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ROLL_LESS, new KeyboardBinding(KEYBOARD_KEY_E)));
	m_primaryBindings.insert(std::pair<Action, Binding*>(THRUST_MORE, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(THRUST_LESS, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ZOOM_MORE, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ZOOM_LESS, new Binding()));

	std::vector<SolaireInputCode> vec;
	vec.push_back(KEYBOARD_KEY_CONTROL);
	vec.push_back(KEYBOARD_KEY_K);
	m_primaryBindings.insert(std::pair<Action, Binding*>(ACTION_1, new KeyboardBinding(vec)));
	vec.clear();
	m_primaryBindings.insert(std::pair<Action, Binding*>(ACTION_2, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ACTION_3, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ACTION_4, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ACTION_5, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ACTION_6, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ACTION_7, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ACTION_8, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ACTION_9, new Binding()));
	m_primaryBindings.insert(std::pair<Action, Binding*>(ACTION_10, new Binding()));


	m_secondaryBindings.insert(std::pair<Action, Binding*>(GAME_MENU, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(PITCH_MORE, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(PITCH_LESS, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(YAW_MORE, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(YAW_LESS, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ROLL_MORE, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ROLL_LESS, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(THRUST_MORE, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(THRUST_LESS, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ZOOM_MORE, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ZOOM_LESS, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ACTION_1, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ACTION_2, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ACTION_3, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ACTION_4, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ACTION_5, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ACTION_6, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ACTION_7, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ACTION_8, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ACTION_9, new Binding()));
	m_secondaryBindings.insert(std::pair<Action, Binding*>(ACTION_10, new Binding()));
	*/
}

KeyBindings::~KeyBindings()
{
	cleanup();
}

void KeyBindings::readBindings(char* filename)
{
	//io::IXMLReader* xml = System::get().getDevice()->getFileSystem()->createXMLReader(filename);
	io::IXMLReaderUTF8* xml = System::get().getDevice()->getFileSystem()->createXMLReaderUTF8(filename);
	SolaireInputCode code = INPUT_NONE;
	std::vector<SolaireInputCode> codes;
	Action action = NOT_AN_ACTION_COUNT;
	BindingType type = BINDING_NOTYPE;
	bool primary = true;

	while(xml && xml->read())
	{	
		//const wchar_t* name = xml->getNodeName();
		//const char* name = xml->getNodeName();
		switch(xml->getNodeType())
		{
			case io::EXN_ELEMENT_END:
				if(!_stricmp("binding", xml->getNodeName()))
				//if(!_wcsicmp(L"binding", xml->getNodeName()))
				{
					switch(type)
					{
						case BINDING_KEYBOARD:
						{
							std::pair<Action, Binding*> p(action, new KeyboardBinding(codes));
							if(primary)
								m_primaryBindings.insert(p);
							else
								m_secondaryBindings.insert(p);
						
							break;
						}
						case BINDING_MOUSE:
						{
							std::pair<Action, Binding*> p(action, new MouseBinding(code));
							if(primary)
								m_primaryBindings.insert(p);
							else
								m_secondaryBindings.insert(p);
							break;
						}
						case BINDING_JOYSTICK:
						{
							std::pair<Action, Binding*> p(action, new JoysticBinding(codes));
							if(primary)
								m_primaryBindings.insert(p);
							else
								m_secondaryBindings.insert(p);
							break;
						}
					}
					action = NOT_AN_ACTION_COUNT;
					type = BINDING_NOTYPE;
					code = INPUT_NONE;
					codes.clear();
				}
				break;
			case io::EXN_ELEMENT:
				if(!_stricmp("key", xml->getNodeName()))
				//if(!_wcsicmp(L"key", xml->getNodeName()))
				{
					type = BINDING_KEYBOARD;
					if(sscanf_s(xml->getAttributeValue("code"), "%ud", &code) != EOF)
					//if(wscanf_s(xml->getAttributeValue(L"code"), "%ud", &code) != EOF)
					{
						//if(_stricmp(Utilities::getPrintable(INPUT_NONE), Utilities::getPrintable(code)))
						if(code > KEYBOARD_FIRST && code < KEYBOARD_LAST && _wcsicmp(Utilities::getPrintable(INPUT_NONE), Utilities::getPrintable(code)))						
							codes.push_back(code);
						else
						{
							System::get().log("bindings.xml: detected invalid key binding for keyboard");
							type = BINDING_NOTYPE;
							code = INPUT_NONE;
							action = NOT_AN_ACTION_COUNT;
						}
					}
					else
					{
						System::get().log("bindings.xml: detected invalid key binding for keyboard");
						type = BINDING_NOTYPE;
						code = INPUT_NONE;
						action = NOT_AN_ACTION_COUNT;
					}
				}
				else if(!_stricmp("mouse", xml->getNodeName()))
				//else if(!_wcsicmp(L"mouse", xml->getNodeName()))
				{
					type = BINDING_MOUSE;
					if(sscanf_s(xml->getAttributeValue("code"), "%ud", &code) == EOF)
					//if(wscanf_s(xml->getAttributeValue(L"code"), "%ud", &code) != EOF)
					{
						System::get().log("bindings.xml: detected invalid key binding for mouse");
						type = BINDING_NOTYPE;
						code = INPUT_NONE;
						action = NOT_AN_ACTION_COUNT;
					}
					if( !(code > MOUSE_FIRST && code < MOUSE_LAST) )
					{
						System::get().log("bindings.xml: detected invalid key binding for mouse");
						type = BINDING_NOTYPE;
						code = INPUT_NONE;
						action = NOT_AN_ACTION_COUNT;
					}
				}
				else if(!_stricmp("joystick", xml->getNodeName()))
				//if(!_wcsicmp(L"joystick", xml->getNodeName()))
				{
					type = BINDING_JOYSTICK;
					if(sscanf_s(xml->getAttributeValue("code"), "%ud", &code) != EOF)
					//if(wscanf_s(xml->getAttributeValue(L"code"), "%ud", &code) != EOF)
					{
						//if(_stricmp(Utilities::getPrintable(INPUT_NONE), Utilities::getPrintable(code)))
						if(code > JOYSTICK_FIRST && code < JOYSTICK_LAST && _wcsicmp(Utilities::getPrintable(INPUT_NONE), Utilities::getPrintable(code)))						
							codes.push_back(code);
						else
						{
							System::get().log("bindings.xml: detected invalid key binding for joystick");
							type = BINDING_NOTYPE;
							code = INPUT_NONE;
							action = NOT_AN_ACTION_COUNT;
						}
					}
					else
					{
						System::get().log("bindings.xml: detected invalid key binding for joystick");
						type = BINDING_NOTYPE;
						code = INPUT_NONE;
						action = NOT_AN_ACTION_COUNT;
					}
				}
				else if(!_stricmp("primary", xml->getNodeName()))
				//else if(!_wcsicmp(L"primary", xml->getNodeName()))
				{
					primary = true;
				}
				else if(!_stricmp("secondary", xml->getNodeName()))
				//else if(!_wcsicmp(L"secondary", xml->getNodeName()))
				{
					primary = false;
				}
				else if(!_stricmp("binding", xml->getNodeName()))
				//else if(!_wcsicmp(L"binding", xml->getNodeName()))
				{
					if(sscanf_s(xml->getAttributeValue("action"), "%ud", &action) == EOF)
					//if(wscanf_s(xml->getAttributeValue(L"action"), "%ud", &action) != EOF)
						action = NOT_AN_ACTION_COUNT;
					if(! (action > NOT_AN_ACTION_FIRST && action < NOT_AN_ACTION_COUNT))
					{
						System::get().log("bindings.xml: detected invalid action number");
						action = NOT_AN_ACTION_COUNT;
					}
				}
				else if(!_stricmp("bindings", xml->getNodeName()))
				//else if(!_wcsicmp(L"bindings", xml->getNodeName()))
				{
					cleanup();
				}
				break;
		}
	}
	delete xml;
}

void KeyBindings::saveBindings(char* filename)
{
	//io::IXMLWriter* xml = System::get().getDevice()->getFileSystem()->createXMLWriter(filename);
	io::IXMLWriter* xml = System::get().getDevice()->getFileSystem()->createXMLWriter(filename);
	xml->writeXMLHeader();
	xml->writeElement(L"bindings");
	xml->writeLineBreak();

	xml->writeElement(L"primary");
	xml->writeLineBreak();
	for(std::map<Action, Binding*>::const_iterator it = m_primaryBindings.begin(); it != m_primaryBindings.end(); ++it)
	{
		core::stringw action(it->first);
		xml->writeElement(L"binding", false, L"action", action.c_str());
		xml->writeLineBreak();
		it->second->writeToXML(xml);
		xml->writeClosingTag(L"binding");
		xml->writeLineBreak();
	}
	xml->writeClosingTag(L"primary");
	xml->writeLineBreak();

	xml->writeElement(L"secondary");
	xml->writeLineBreak();
	for(std::map<Action, Binding*>::const_iterator it = m_secondaryBindings.begin(); it != m_secondaryBindings.end(); ++it)
	{
		core::stringw action(it->first);
		xml->writeElement(L"binding", false, L"action", action.c_str());
		xml->writeLineBreak();
		it->second->writeToXML(xml);
		xml->writeClosingTag(L"binding");
		xml->writeLineBreak();
	}
	xml->writeClosingTag(L"secondary");
	xml->writeLineBreak();


	xml->writeClosingTag(L"bindings");
	xml->writeLineBreak();
	xml->drop();
}

bool KeyBindings::isTriggered(const Action action) const
{
	bool primary = false, secondary = false;
	std::map<Action, Binding*>::const_iterator it = m_primaryBindings.find(action);
	if(it != m_primaryBindings.end())
	{
		primary = it->second->isTriggered();
		if(primary)
			return true;
	}
	it = m_secondaryBindings.find(action);
	if(it != m_secondaryBindings.end())
	{
		secondary = it->second->isTriggered();
	}
	return primary || secondary;
}

float KeyBindings::getValue(const Action action) const
{
	std::map<Action, Binding*>::const_iterator it = m_primaryBindings.find(action);
	if(it != m_primaryBindings.end() && it->second->isTriggered())
	{
		return it->second->getValue();
	}
	it = m_secondaryBindings.find(action);
	if(it != m_secondaryBindings.end())
	{
		return it->second->getValue();
	}
	return 0.0f;
}

void KeyBindings::rebind(const Action action, Binding* binding, const bool primary)
{
	std::map<Action, Binding*>* whichMap;
	if(primary)
		whichMap = &m_primaryBindings;
	else
		whichMap = &m_secondaryBindings;

	std::map<Action, Binding*>::iterator it = whichMap->find(action);
	if(it != whichMap->end())
	{
		delete it->second;
	}
	whichMap->erase(action);
	whichMap->insert(std::pair<Action, Binding*>(action, binding));
}

Binding* KeyBindings::rebindEmpty(const Action action, const BindingType type, const bool primary)
{
	std::map<Action, Binding*>* whichMap;
	if(primary)
		whichMap = &m_primaryBindings;
	else
		whichMap = &m_secondaryBindings;

	std::map<Action, Binding*>::iterator it = whichMap->find(action);
	if(it != whichMap->end())
	{
		delete it->second;
	}
	whichMap->erase(action);
	Binding* n = NULL;
	switch(type)
	{
		case BINDING_KEYBOARD:
		{
			n = new KeyboardBinding(INPUT_NONE);
			n->clear();
			break;
		}
		case BINDING_MOUSE:
		{
			n = new MouseBinding(INPUT_NONE);
			break;
		}
		case BINDING_NOTYPE:
		{
			n = new Binding;
			break;
		}
		case BINDING_JOYSTICK:
			n = new JoysticBinding(INPUT_NONE);
			n->clear();
			break;
	}
	if(n)
		whichMap->insert(std::pair<Action, Binding*>(action, n));

	return n;
}

core::stringw KeyBindings::getBindingFor(const Action action, const bool primary)
{
	std::map<Action, Binding*>* whichMap;
	if(primary)
		whichMap = &m_primaryBindings;
	else
		whichMap = &m_secondaryBindings;
	
	std::map<Action, Binding*>::iterator it = whichMap->find(action);
	if(it != whichMap->end())
	{
		return it->second->toString();
	}
	return Utilities::getPrintable(INPUT_NONE);
}

Binding* KeyBindings::getBinding(const Action action)
{
	Binding* b = NULL;
	if(! (b = getBinding(action, true)))
		return getBinding(action, false);
	return b;
}

Binding* KeyBindings::getBinding(const Action action, const bool primary)
{
	std::map<Action, Binding*>* whichMap;
	if(primary)
		whichMap = &m_primaryBindings;
	else
		whichMap = &m_secondaryBindings;
	
	std::map<Action, Binding*>::iterator it = whichMap->find(action);
	if(it != whichMap->end())
	{
		return it->second;
	}
	return NULL;
}

Action KeyBindings::getActionFor(Binding* binding) const
{
	for(std::map<Action, Binding*>::const_iterator it = m_primaryBindings.begin(); it != m_primaryBindings.end(); ++it)
	{
		if(it->second == binding)
			return it->first;
	}
	for(std::map<Action, Binding*>::const_iterator it = m_secondaryBindings.begin(); it != m_secondaryBindings.end(); ++it)
	{
		if(it->second == binding)
			return it->first;
	}
	return NOT_AN_ACTION_COUNT;
}

bool KeyBindings::onKeyEvent(const irr::SEvent::SKeyInput& evt)
{	
	return false;
}

bool KeyBindings::onMouseEvent(const irr::SEvent::SMouseInput& evt)
{
	if(evt.Event == EMIE_MOUSE_WHEEL)
	{
		m_mouseWheel = evt.Wheel;
	}
	else if(evt.Event == EMIE_MOUSE_MOVED)
	{
		m_mouseDeltaX = static_cast<float>(evt.X - m_lastMouseX);
		m_mouseDeltaY = static_cast<float>(evt.Y - m_lastMouseY);

		m_lastMouseX = evt.X;
		m_lastMouseY = evt.Y;
	}
	return false;
}
	
bool KeyBindings::onJoystickEvent(const irr::SEvent::SJoystickEvent& evt)
{

	return false;
}

float KeyBindings::getMousePosX() const
{
	return static_cast<float>(m_lastMouseX);
}

float KeyBindings::getMouseDeltaX() const
{
	return m_mouseDeltaX;
}

float KeyBindings::getMousePosY() const
{
	return static_cast<float>(m_lastMouseY);
}

float KeyBindings::getMouseDeltaY() const
{
	return m_mouseDeltaY;
}

float KeyBindings::getMouseWheelValue() const
{
	return m_mouseWheel;
}

void KeyBindings::cleanup()
{
	for(std::map<Action, Binding*>::const_iterator it = m_primaryBindings.begin(); it != m_primaryBindings.end(); ++it)
	{
		delete it->second;
	}
	m_primaryBindings.clear();

	for(std::map<Action, Binding*>::const_iterator it = m_secondaryBindings.begin(); it != m_secondaryBindings.end(); ++it)
	{
		delete it->second;
	}
	m_secondaryBindings.clear();
}

void KeyBindings::update(const float dt)
{
	for (auto i = m_primaryBindings.begin(); i != m_primaryBindings.end(); i++)
	{
		(*i).second->update(dt);
	}
	for (auto i = m_secondaryBindings.begin(); i != m_secondaryBindings.end(); i++)
	{
		(*i).second->update(dt);
	}
}

