#pragma once
#include "PhysicsParams.h"
//#include "CSLock.h"
#include <vector3d.h>
#include <vector>

using irr::core::vector3df;

class PhysicsObject
{
private:
	vector3df m_ThrustForce;
	std::vector<vector3df> m_ImpulseForces;
	vector3df m_Position; 
	vector3df m_Velocity; 
	vector3df m_Acceleration;
	double m_RemainingLifespan; 
	bool m_NeedsCleanup; 
	void IntegrationUpdate(const float deltaTime);
public:
	PhysicsObject() : m_NeedsCleanup(false), m_RemainingLifespan(0.0) {} 
	~PhysicsObject() {} 
	PhysicsParams m_Parameters;
	void Update(const float deltaTime);

	void ApplyThrust(const vector3df &direction, const float thrustVal);
	void ApplyImpulse(const vector3df &impulse);
	void SetParameters(const PhysicsParams &params); 
	void Init(const PhysicsParams &params);
	void MarkForCleanup()
	{
		m_NeedsCleanup = true;
	}
	bool CheckCleanupState()
	{
		return m_NeedsCleanup;
	}
	vector3df &GetPosition()
	{
		return m_Position;
	}
	vector3df &GetVelocity()
	{
		return m_Velocity;
	}
	void SetVelocity(vector3df& vel)
	{
		m_Velocity = vel; 
	}
	void SetPosition(vector3df& pos);
	void Reset(const vector3df& pos); 
};

//float PenetrationDistance = MinimumDistance - Distance; 
	////float TotalMass = ObjectA->m_Parameters.Mass + ObjectB->m_Parameters.Mass;
	//vector3df CorrectionDirection(-ObjectA->GetVelocity());
	//CorrectionDirection.normalize(); 
	//CorrectionDirection *= PenetrationDistance * 0.5f;
	//ObjectA->SetPosition(ObjectA->GetPosition() + CorrectionDirection * deltaTime);

	//CorrectionDirection = -ObjectB->GetVelocity();
	//CorrectionDirection.normalize(); 
	//CorrectionDirection *= PenetrationDistance * 0.5f;
	//ObjectB->SetPosition(ObjectB->GetPosition() + CorrectionDirection * deltaTime);