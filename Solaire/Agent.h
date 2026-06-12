#pragma once
#include <irrString.h>
#include <irrlicht.h>
#include <vector3d.h>
#include "Actuator.h"
#include "ShaderIncludes.h"
using irr::core::stringw; 
using irr::core::vector3df;

class irr::gui::IGUIStaticText;

class SpaceObject; 
class Actuator; 
class LogicScene;

class Agent
{

private:
	void updateTargetOutline(const float dt);

protected:
	stringw m_Name; 
	unsigned int m_Mask;
	unsigned int m_Score;
	unsigned int m_AgentID; 
	SpaceObject* m_CurrentObject;
	LogicScene* m_ParentScene; 

	// HUD Data
	HUDShader* m_hudshader;
	HUDShader* m_targetshader;
	float* m_Shield;
	float* m_Armour; 
	irr::scene::IMeshSceneNode* m_reticule;
	irr::gui::IGUIStaticText* m_TargetName; 
	float m_TargetShield;
	float m_TargetArmour; 

	// Lock indicators

	irr::scene::IMeshSceneNode* m_Arrow; 
	irr::scene::IMeshSceneNode* m_TargetDataNode; 


	// TODO Delete old lock marker code
	//LockShader* m_LockShader; 
	//vector3df m_TopLeft, m_BottomRight, m_TargetDirection;
	//float m_LockState;

public:
	Agent() : m_Score(0), m_AgentID(0), m_CurrentObject(NULL), m_Mask(0), m_ParentScene(NULL), m_Shield (NULL), m_Armour (NULL), m_hudshader(NULL), m_targetshader(NULL), m_reticule(NULL), m_TargetName (NULL), m_Arrow(NULL), m_TargetShield(0.0f), m_TargetArmour(0.0f), m_TargetDataNode(NULL)
	{
	}
	virtual ~Agent() 
	{
		clean();
		//if (m_LockShader)
		//{
		//	delete m_LockShader;
		//	m_LockShader = NULL;
		//}
	} 

	SpaceObject* GetSpaceObject()
	{
		return m_CurrentObject;
	}
	void SetSpaceObject(SpaceObject* obj)
	{
		m_CurrentObject = obj; 
	}
	const stringw& GetName()
	{
		return m_Name; 
	}
	void SetName(const stringw& name)
	{
		m_Name = name; 
	}
	const unsigned int GetID()
	{
		return m_AgentID;
	}
	void SetID(const unsigned int id)
	{
		m_AgentID = id;
	}
	const unsigned int GetMask()
	{
		return m_Mask;
	}
	void SetMask(const unsigned int mask)
	{
		m_Mask = mask; 
	}
	LogicScene* GetParentScene()
	{
		return m_ParentScene; 
	}
	void SetParentScene(LogicScene* scene)
	{
		m_ParentScene = scene;
	}

	const unsigned int GetScore() 
	{
		return m_Score; 
	}

	virtual void Init() {} 
	virtual void Update(float dt) {} 
	virtual void clean();
	void InitGUI();
	void UpdateGUI(float dt);
};