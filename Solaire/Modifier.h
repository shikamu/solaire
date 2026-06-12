#pragma once

enum ModifierType
{
	NONE, POS_PERM, NEG_PERM, POS_TEMP, NEG_TEMP
};


class Modifier
{
private:
protected: 
	float *m_Value; 
	float m_Modifier; 
	float m_Lifespan;
	bool m_NeedsDeletion;
	ModifierType m_Type;
public:
	Modifier() : m_Value(0), m_Modifier (0.0), m_Lifespan(1.0f), m_NeedsDeletion (false), m_Type(NONE) {}
	virtual ~Modifier() {}
	virtual void Update(const float dt) 
	{
		m_Lifespan -= dt;
		if (m_Lifespan < 0.0f) m_NeedsDeletion = true; 
	} 
	virtual void Initiate() {}
	virtual void Terminate() {} 
	void Pointer(float *p)
	{
		m_Value = p;
	} 
	const float *Pointer()
	{
		return m_Value; 
	}
	void ModifierValue(const float m)
	{
		m_Modifier = m; 
	}
	const float ModifierValue()
	{
		return m_Modifier; 
	}
	void Lifespan(const float t) 
	{
		m_Lifespan = t; 
	}
	const float Lifespan()
	{
		return m_Lifespan; 
	}
	void Type(const ModifierType Type)
	{
		m_Type = Type; 
	}
	const ModifierType Type()
	{
		return m_Type; 
	}
	const bool NeedsDeletion()
	{
		return m_NeedsDeletion; 
	}
};