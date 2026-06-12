#pragma once
#include <vector3d.h>
#include <quaternion.h>

class SpaceObject; 

namespace irr
{
	namespace scene
	{
		class IMeshSceneNode;
		class ISceneManager;
	}
}

struct ActuatorOutput;

struct ActuatorData
{
	unsigned int ButtonMask; 
	float PitchVal, YawVal, RollVal, ThrustVal;
	ActuatorData() : ButtonMask(0), PitchVal(0.0f), YawVal(0.0f), RollVal(0.0f), ThrustVal(0.0f) {}
	ActuatorData(const unsigned int b, const float p, const float y, const float r, const float t) : ButtonMask(b), PitchVal(p), YawVal(y), RollVal(r), ThrustVal(t) {}
	
	ActuatorOutput toOutput(const unsigned int id);
};

struct ActuatorOutput
{
	unsigned int shipID;
	unsigned int ButtonMask; 
	float PitchVal, YawVal, RollVal, ThrustVal;
	ActuatorOutput(const unsigned int id) : shipID(id), ButtonMask(0), PitchVal(0.0f), YawVal(0.0f), RollVal(0.0f), ThrustVal(0.0f) {}

	ActuatorOutput(const unsigned int id, const ActuatorData& data) : shipID(id), ButtonMask(data.ButtonMask), PitchVal(data.PitchVal), YawVal(data.YawVal), RollVal(data.RollVal), ThrustVal(data.ThrustVal) {}

	ActuatorData toData() const
	{
		return ActuatorData(ButtonMask, PitchVal, YawVal, RollVal, ThrustVal);
	}
};

class Actuator
{
public:
	Actuator();
	Actuator(irr::core::quaternion& current); 

	virtual ~Actuator();

	int create(irr::scene::IMeshSceneNode* inNode, irr::scene::ISceneManager* inScene);
	
	virtual void update(SpaceObject* host, const float dt);
	
	const irr::core::quaternion& getQCurrent() const
	{
		return m_QCurrent;
	}

	const irr::core::quaternion& getQAll() const
	{
		return m_QAll;
	}

	const irr::core::vector3df& getPosition() const
	{
		return m_Pos;
	}

	const irr::core::vector3df& getRotation() const
	{
		return m_Rot;
	}

	const irr::scene::IMeshSceneNode* getNode() const
	{
		return m_Object;
	}
		
	bool isFiring() const
	{
		return m_Firing; 
	}
	
	bool isSecondaryPressed() const
	{
		return m_Secondary;
	}
	
	bool isTertiaryPressed() const
	{
		return m_Tertiary;
	}
	
	virtual float getZoom() const
	{
		return 1.0f;
	}

	irr::core::vector3df& GetDirection()
	{
		return m_Direction; 
	}
	float GetThrust()
	{
		return m_Thrust;
	}
	irr::core::vector3df GetControlValues() // Return by value is intentional - Vector only constructed within function 
	{
		irr::core::vector3df Output; 
		Output.X = m_FPitch;
		Output.Y = m_FYaw;
		Output.Z = m_FRoll; 
		return Output; 
	}

	virtual Actuator* GetReplacement();

	bool NeedsReplacement() const;

	void SetNeedReplacement(const bool b);

	bool CheckButton(unsigned int value);
	bool CheckActType(unsigned short type); 

protected:
	irr::scene::IMeshSceneNode *m_Object;
	irr::scene::ISceneManager *m_Scene; 
	irr::core::vector3df m_Pos, m_Rot, m_Movement;
	irr::core::quaternion m_QCurrent; 
	irr::core::quaternion m_QPitch, m_QRoll, m_QYaw, m_QAll;
	float m_FPitch, m_FRoll, m_FYaw, m_FThrust;
	bool m_Firing, m_Secondary, m_Tertiary;
	float m_Thrust; 
	irr::core::vector3df m_Direction;
	unsigned int m_ButtonMask;

private:
	bool m_needsReplacement;
};

