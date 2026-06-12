#pragma once
#include <vector3d.h>
//#include "Modifier.h"
//#include "Shader.h"
//#include "LogicConstants.h"

using irr::core::vector3df;

class SpaceObject;
class SpaceObjectFactory; 

enum ModuleType;
enum ModuleSize;
class LogicModule
{
protected:
	SpaceObject* m_Source;
	SpaceObject* m_Target; 
	float m_Cooldown, m_CurrentCooldown;
	unsigned short m_ActivationType;
public:
	LogicModule() : m_Source (NULL), m_Target (NULL), m_Cooldown (0.0f), m_CurrentCooldown (0.0f), m_ActivationType(1) {}
	virtual ~LogicModule() {}
	void SetCooldown(float time)
	{
		m_Cooldown = time;
	}
	float CooldownFraction()
	{
		return m_CurrentCooldown / m_Cooldown; 
	}
	void SetSource(SpaceObject* source)
	{
		m_Source = source;
	}
	void SetTarget(SpaceObject* target)
	{
		m_Target = target; 
	}
	void SetActType(unsigned short type)
	{
		m_ActivationType = type;
	}
	unsigned short getActType()
	{
		return m_ActivationType;
	}
	virtual bool Activate() {return false;}
	virtual bool Activate(SpaceObject* target) {return false;} 
	virtual void Update(float dT) {}
};

class ModuleNode
{
private:
	bool m_ModuleTransferred;
	ModuleNode& operator= (const ModuleNode& other) {}

	// NOTE: Fitted modules should only be assigned once the node is placed in destination vector etc
public:
	ModuleType Type;
	ModuleSize Size;
	unsigned short ActivationType;
	vector3df RelativePosition; 
	vector3df RelativeRotation;
	LogicModule* FittedModule;
	ModuleNode();
	
	ModuleNode(const ModuleNode& other);

	~ModuleNode() 
	{
		if (FittedModule && !m_ModuleTransferred) 
		{
			delete FittedModule;
		}
	} 
};