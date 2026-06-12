#pragma once
#include <map>
#include <vector>
#include <irrString.h>
#include "IEventListener.h"

enum SolaireInputCode;
enum Action;

namespace irr
{
	namespace io
	{
		class IXMLWriter;
	}
}

enum BindingType
{
	BINDING_NOTYPE,
	BINDING_KEYBOARD,
	BINDING_MOUSE,
	BINDING_JOYSTICK
};

class Binding
{
public:
	virtual ~Binding();
	//default behaviour matches that of SolaireInputCode::INPUT_NONE

	//returns whether the binding is triggered (e.g. pressed for a keyboard binding)
	virtual bool isTriggered() const;

	//returns a value that represents to what degree the key is triggered. this is mostly relevant in the case of joystick or mouse but it can also be applied to keyboard
	//this function will usually return 0.0f if the key is not triggered
	virtual float getValue() const;

	//returns the type of the binding
	virtual BindingType getType() const;

	//writes this binding to anx XML document
	virtual void writeToXML(irr::io::IXMLWriter*) const;

	//returns the string representation of this binding
	virtual irr::core::stringw toString() const;

	//reset this binding to the given code, or in cases where multiple input code can make a binding, adds the given code to the list of this binding
	virtual void push(const SolaireInputCode code);

	//resets the binding to nothing
	virtual void clear();


	virtual SolaireInputCode getCode() const; //TODO this is not enough as some types of Binding can have multiple codes
	
	//this method shouldn't be needed as it breaks the genericity a little bit but it simplifies some of the coding
	virtual bool isAxis() const;
	virtual void update(const float dt);
};

class KeyboardBinding : public Binding
{
private:
	float m_Value;
public:
	KeyboardBinding(const SolaireInputCode key);
	KeyboardBinding(const std::vector<SolaireInputCode>& keys);
	virtual ~KeyboardBinding();
	bool isTriggered() const;
	float getValue() const;
	BindingType getType() const;
	void writeToXML(irr::io::IXMLWriter*) const;
	irr::core::stringw toString() const;
	void push(const SolaireInputCode key);
	void clear();
	void update(const float dt);

	SolaireInputCode getCode() const; //TODO this is not enough as some types of Binding can have multiple codes
protected:
	std::vector<SolaireInputCode> m_keys;
};

class MouseBinding : public Binding
{
public:
	MouseBinding(SolaireInputCode code);
	bool isTriggered() const;
	float getValue() const;
	BindingType getType() const;
	void writeToXML(irr::io::IXMLWriter*) const;
	irr::core::stringw toString() const;
	void push(const SolaireInputCode key);
	void clear();
	void update(const float dt); 
	SolaireInputCode getCode() const; //TODO this is not enough as some types of Binding can have multiple codes
private:
	SolaireInputCode m_code;
	float m_PosX, m_PosY;
	float m_ControlX, m_ControlY; 
	float m_dX, m_dY;
	bool m_IsReady; 
};

class JoysticBinding : public KeyboardBinding
{
public:
	JoysticBinding(const SolaireInputCode key);
	JoysticBinding(const std::vector<SolaireInputCode>& keys);
	bool isTriggered() const;
	float getValue() const;
	BindingType getType() const;
	void writeToXML(irr::io::IXMLWriter*) const;
	bool isAxis() const;
private:
	float deadZoneCompensate(const float Value, const float deadzone = 0.2f) const;
};

class KeyBindings: public IEventListener
{
public:
	KeyBindings();
	~KeyBindings();

	void readBindings(char* filename = "data/bindings.xml");
	void saveBindings(char* filename = "data/bindings.xml");

	bool isTriggered(const Action action) const;

	float getValue(const Action action) const;

	void rebind(const Action action, Binding* binding, const bool primary);

	Binding* rebindEmpty(const Action action, const BindingType type, const bool primary);

	irr::core::stringw getBindingFor(const Action action, const bool primary);
	Binding* getBinding(const Action action);//this will check in primary first and if it can't find it, look in secondary
	Binding* getBinding(const Action action, const bool primary);
	Action getActionFor(Binding* binding) const;

	bool onKeyEvent(const irr::SEvent::SKeyInput& evt);

	bool onMouseEvent(const irr::SEvent::SMouseInput& evt);
	
	bool onJoystickEvent(const irr::SEvent::SJoystickEvent& evt);

	float getMousePosX() const;
	float getMouseDeltaX() const;

	float getMousePosY() const;
	float getMouseDeltaY() const;

	float getMouseWheelValue() const;

	void update(const float dt);

private:

	void cleanup();

	std::map<Action, Binding*> m_primaryBindings;
	std::map<Action, Binding*> m_secondaryBindings;

	irr::s32 m_lastMouseX, m_lastMouseY;
	float m_mouseDeltaX, m_mouseDeltaY, m_mouseWheel;
};

