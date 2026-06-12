#include <cmath> 
#include <iostream>
#include "PropertyState.h"
#include "Modifier.h"
#include "LogicConstants.h"


using std::max;
using std::min;

PropertyState::PropertyState() : m_Max (1.0f), m_Min(0.0f), m_Base(0.5f), m_Current(0.5f), m_Unmodified (0.5f), m_ID(PROP_NONE), m_CurrentFraction(1.0f) 
{
}

PropertyState::~PropertyState()
{
	for (auto i = m_Modifiers.begin(); i < m_Modifiers.end(); ++i)
		delete (*i);
}

void PropertyState::ApplyModifiers(const float deltaTime)
{
	int positiveCount = 0;
	int negativeCount = 0; 
	float positiveModifier = 0.0f; 
	float negativeModifier = 0.0f; 
	float totalModifier = 1.0f; 
	m_PropertyLock.Lock();
	for (auto i = m_Modifiers.begin(); i < m_Modifiers.end();)
	{
		(*i)->Update(deltaTime); 
		switch((*i)->Type())
		{
		case POS_TEMP:
			positiveCount++;
			positiveModifier += (*i)->ModifierValue()/(float)positiveCount;
			break;
		case NEG_TEMP:
			negativeCount++;
			negativeModifier += (*i)->ModifierValue()/(float)negativeCount;
			break;
		case POS_PERM:
			DirectAlteration((*i)->ModifierValue());
			break;
		case NEG_PERM:
			DirectAlteration(-(*i)->ModifierValue());
			break;
		default:
			break; 
		}

		//std::cout << "Modifier Applied: " << (*i)->ModifierValue() << std::endl;
		if ((*i)->NeedsDeletion())
		{
			delete (*i);
			i = m_Modifiers.erase(i);
		}
		else i++;
	}
	m_PropertyLock.Unlock();
	totalModifier += positiveModifier - negativeModifier;

	m_Current = max(m_Min, min(m_Max, m_Unmodified * totalModifier)); 
	m_CurrentFraction = (m_Current - m_Min) / (m_Max - m_Min);

}

void PropertyState::DirectAlteration(float value)
{
	m_Unmodified = max(m_Min, min(m_Max, m_Unmodified + value));
}

void PropertyState::Reset()
{
	m_PropertyLock.Lock();
	for (auto i = m_Modifiers.begin(); i < m_Modifiers.end(); i++)
	{
		//delete (*i);
	}
		
	m_Unmodified = m_Base; 
	m_PropertyLock.Unlock();
}