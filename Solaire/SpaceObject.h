#pragma once
#pragma warning(disable : 4355)//we aren't calling any virtual functions at the point where the warning is occurring

#include <vector>
#include <map>
#include <irrString.h>
#include <vector3d.h>
//#include "Shader.h"
#include "PhysicsListener.h"
#include "PropertyState.h"
//#include "LogicConstants.h"
//#include "LogicModule.h"
//#include "TargettingModule.h"
//#include "LockPointer.h"

using std::vector;
using irr::core::stringw; 
using std::map;
using irr::core::vector3df; 

class PropertyState;
class LogicScene; 
class Actuator; 
class PhysicsObject;
class RenderObject; 
class TargettingModule;
class ModuleNode; 
class SpaceObjectShell; 

enum PropertyTypeID;

template<class T> class LockPointer;

namespace irr
{
	namespace scene
	{
		class IMeshSceneNode;
	}
}

/*
- Static Data 
	- PointerHub
		- Parent Render Node
		- Physics Object
		- Actuator
		- Parent Logic Scene
	- Name 
	- Ship type
	- Global ID
- Dynamic Data 
	- Property States
	- Position
	- Velocity
	- Rotation 
- Module Nodes
- Targetting 
	- Soft Target list
	- Selected Soft Target
	- Selected Hard Target
*/


class SpaceObject : public PhysicsListener
{
private:
	// Static Data
	////////////////////
	stringw m_Name; 
	stringw m_ShipType;
	unsigned int m_AgentID; 
	////////////////////

	// Pointer Hub
	////////////////////
	LogicScene* m_ParentScene;
	RenderObject* m_RenderObj; 
	LockPointer<PhysicsObject>* m_PhysicsObj; 
	Actuator* m_Actuator; 
	TargettingModule* m_Targetter; // Will be deleted inside space object
	LockPointer<SpaceObjectShell>* m_Shell; 
	////////////////////

	//Target Pointers
	////////////////////
	SpaceObject* m_HardTarget; 
	SpaceObject* m_SoftTarget;
	SpaceObject* m_CollisionTarget;
	////////////////////

	// Dynamic Data
	////////////////////
	map<PropertyTypeID, PropertyState> m_Properties;
	vector3df m_Position, m_Rotation, m_Velocity;
	bool m_NeedsDeletion;
	////////////////////

	// Network interpolation (client side)
	////////////////////
	vector3df m_NetTargetPosition, m_NetTargetRotation;	//latest server-reported transform
	bool m_HasNetTarget;								//false until the first server update arrives
	////////////////////

	// Shield/Armour Data
	////////////////////
	vector3df m_ShieldImpactDirection;
	float m_ShieldImpactTime;
	float m_ShieldRemaining;
	float m_ArmourRemaining; 
	////////////////////
	
	SpaceObject& operator= (const SpaceObject& other){}
	SpaceObject(const SpaceObject& other){}
public:


	unsigned int ID, ObjectMask; // Global ID and mask
	vector<ModuleNode*> Modules; // Module Node List, modules themselves will be deleted within the node


	CSLock m_ModuleListLock; 
	SpaceObject() :
		PhysicsListener(this), m_ParentScene (NULL),  ID (0), m_RenderObj (NULL), m_Targetter (NULL), m_AgentID(0),
		m_Actuator (NULL), m_PhysicsObj (NULL), m_ShieldImpactTime (0.0f), m_ShieldRemaining (1.0f), m_ArmourRemaining(1.0f),  ObjectMask (0),
		m_NeedsDeletion (false), m_CollisionTarget (NULL), m_HardTarget (NULL), m_SoftTarget (NULL), m_Shell (NULL), m_HasNetTarget (false) {};
	SpaceObject(LogicScene* parent) :
		PhysicsListener(this), m_ParentScene (parent), ID (0), m_RenderObj (NULL), m_Targetter (NULL), m_AgentID(0),
		m_Actuator (NULL), m_PhysicsObj (NULL), m_ShieldImpactTime (0.0f), m_ShieldRemaining (1.0f), m_ArmourRemaining(1.0f), ObjectMask (0),
		m_NeedsDeletion (false), m_CollisionTarget (NULL), m_HardTarget (NULL), m_SoftTarget (NULL), m_Shell (NULL), m_HasNetTarget (false) {};
	
	~SpaceObject();

	// Static Data Getters/Setters
	/////////////////////////
	const stringw& GetName() const
	{
		return m_Name;
	}
	void SetName(const stringw &name)
	{
		m_Name = name; 
	}
	const stringw& GetType() const
	{
		return m_Name;
	}
	void SetType(const stringw &type)
	{
		m_ShipType = type; 
	}
	void SetAgentID(unsigned int id)
	{
		m_AgentID = id;
	}
	unsigned int GetAgentID()
	{
		return m_AgentID;
	}
	const vector3df& GetVelocity()
	{
		return m_Velocity;
	}
	//void AddShader(Shader* shaderinstance)
	//{
	//	m_ShaderContainer.push_back(shaderinstance);
	//}
	/////////////////////////

	// PropertyState Functions
	/////////////////////////
	PropertyState* GetPropertyByLabel(const stringw& name, bool& Exists);

	PropertyState* GetPropertyByID(PropertyTypeID ID);


	PropertyState* GetPropertyByID(PropertyTypeID ID, bool& Exists); 
	void AddPropertyState(const PropertyTypeID type, const PropertyState& state); 
	/////////////////////////

	
	// Pointer Hub Getters/Setters
	/////////////////////////
	LogicScene* GetParentScene()
	{
		return m_ParentScene; 
	}
	void SetParentScene(LogicScene* parent)
	{
		m_ParentScene = parent; 
	}
	RenderObject* GetRenderObject()
	{
		return m_RenderObj;
	}
	void SetRenderObject(RenderObject* node)
	{
		m_RenderObj = node; 
	}
	Actuator* GetActuator()
	{
		return m_Actuator;
	}
	void SetActuator(Actuator* actuator)
	{
		m_Actuator = actuator; 
	}
	LockPointer<PhysicsObject>* GetPhysicsObject()
	{
		return m_PhysicsObj;
	}
	void SetPhysicsObject(LockPointer<PhysicsObject>* object)
	{
		m_PhysicsObj = object; 
	}
	void SetTargetter(TargettingModule *module)
	{
		m_Targetter = module;
	}
	TargettingModule* GetTargetter()
	{
		return m_Targetter; 
	}
	void SetShell(LockPointer<SpaceObjectShell>* shell) 
	{
		m_Shell = shell; 
	}
	LockPointer<SpaceObjectShell>* GetShell()
	{
		return m_Shell; 
	}
	/////////////////////////

	// Targetting Getters/Setters
	/////////////////////////
	SpaceObject* GetHardTarget()
	{
		return m_HardTarget;
	}
	void SetHardTarget(SpaceObject* target)
	{
		m_HardTarget = target;
	}
	SpaceObject* GetSoftTarget()
	{
		return m_SoftTarget;
	}
	void SetSoftTarget(SpaceObject* target)
	{
		m_SoftTarget = target;
	}
	/////////////////////////

	// Shield Shader Getters/Setters
	/////////////////////////////
	void ShieldImpactVector(vector3df& direction)
	{
		m_ShieldImpactDirection = direction; 
	}
	void SetRemainingShields(float value)
	{
		m_ShieldRemaining = value; 
	}
	void SetRemainingArmour(float value)
	{
		m_ArmourRemaining = value; 
	}
	void ShieldImpactTimeReset()
	{
		m_ShieldImpactTime = 1.0f; 
	}
	void SetImpactTime(float value)
	{
		m_ShieldImpactTime = value; 
	}
	vector3df* ShieldImpactVector()
	{
		return &m_ShieldImpactDirection;
	}

	float* ShieldImpactTime()
	{
		return &m_ShieldImpactTime; 
	}
	float* ShieldRemainingValue()
	{
		return &m_ShieldRemaining;
	}
	float* ArmourRemainingValue()
	{
		return &m_ArmourRemaining; 
	}
	/////////////////////////////



	// Overridden PhysicsListener Functions
	/////////////////////////////
	
	void OnDestroy();
	void OnCreate(){}
	void OnCollide(const unsigned int OtherID, const unsigned int OtherParentID);

	/////////////////////////////

	bool NeedsDeletion()
	{
		return m_NeedsDeletion;
	}
	void FlagForDeletion()
	{
		m_NeedsDeletion = true;

	}
	virtual void Update(float dt);

	//Client-side smoothing of networked ships. SetNetworkTarget records the latest server
	//transform (snapping on the first one), and InterpolateToNetworkTarget eases the render
	//object toward it each frame so remote ships glide instead of teleporting per packet.
	void SetNetworkTarget(const vector3df& pos, const vector3df& rot);
	void InterpolateToNetworkTarget(float dt);

	void DeathCheck();
	void Die();
};

