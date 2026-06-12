#pragma once
#include <irrString.h>
#include <vector>
//#include "Modifier.h"
//#include "LogicConstants.h"
#include "CSLock.h"

using irr::core::stringw;
using std::vector;

class Modifier;
enum PropertyTypeID;
class PropertyParams
{
public:
	PropertyParams() {}
	~PropertyParams() {}
	float Max, Min, Base;
	PropertyTypeID ID;
	stringw Label; 
};


class PropertyState
{
private:
protected:
	float m_Max, m_Min, m_Base, m_Current, m_Unmodified, m_CurrentFraction;
	PropertyTypeID m_ID;
	stringw m_Label;
	vector<Modifier*> m_Modifiers;
	CSLock m_PropertyLock;
public:
	PropertyState();
	PropertyState(float max, float min, float base, PropertyTypeID id, const wchar_t* label) 
		: m_Max (max), m_Min(min), m_Base(base), m_Current(base), m_Unmodified (base), m_ID(id), m_Label (label) {}
	~PropertyState();

	PropertyState(const PropertyState& other) 
	{
		m_Max = other.m_Max;
		m_Min = other.m_Min;
		m_Base = other.m_Base;
		m_Current = other.m_Current;
		m_Unmodified = other.m_Unmodified;
		m_CurrentFraction = other.m_CurrentFraction;
		m_ID = other.m_ID;
		m_Label = other.m_Label;
	}

	PropertyState& operator=(const PropertyState& other)
	{
		if (this != &other)
		{
			m_Max = other.m_Max;
			m_Min = other.m_Min;
			m_Base = other.m_Base;
			m_Current = other.m_Current;
			m_Unmodified = other.m_Unmodified;
			m_CurrentFraction = other.m_CurrentFraction;
			m_ID = other.m_ID;
			m_Label = other.m_Label;
		}
		return *this;
	}

	void Max(const float f)
	{
		m_Max = f; 
	}
	float Max()
	{
		return m_Max; 
	}
	void Min(const float f)
	{
		m_Min = f; 
	}
	float Min()
	{
		return m_Min; 
	}

	void Current(const float f)
	{
		m_Current = f; 
	}
	float Current()
	{
		return m_Current; 
	}
	float* CurrentPointer()
	{
		return &m_Current;
	}
	float* CurrentFractionPointer()
	{
		return &m_CurrentFraction;
	}
	void Unmodified(const float f)
	{
		m_Unmodified = f; 
	}
	float Unmodified()
	{
		return m_Unmodified; 
	}
	void ID(const PropertyTypeID id)
	{
		m_ID = id;
	}
	PropertyTypeID ID()
	{
		return m_ID; 
	}
	void Label(stringw& label)
	{
		m_Label = label;
	}
	const stringw& Label()
	{
		return m_Label; 
	}


	void DirectAlteration(float value);
	void AddModifier(Modifier *m)
	{
		m_PropertyLock.Lock();
		m_Modifiers.push_back(m);
		m_PropertyLock.Unlock();
	}
	void ApplyModifiers(const float deltaTime); 
	void Reset();

};