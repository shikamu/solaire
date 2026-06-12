#include "PhysicsObject.h"
#include "PhysicsListener.h"
#include <cmath>

void PhysicsObject::SetParameters(const PhysicsParams &params)
{
	m_Parameters = params;
	m_Parameters.SetInverseMass();
}

void PhysicsObject::Init(const PhysicsParams &params)
{
	SetParameters(params);
	m_Position = params.InitialPosition;
	m_Velocity = params.InitialVelocity;
	m_Acceleration = vector3df(0.0f, 0.0f, 0.0f);
	m_RemainingLifespan = params.Lifespan; 

	if (m_Parameters.Listener) m_Parameters.Listener->OnCreate(); // POSSIBLY NEEDS RELOCATING 
}

void PhysicsObject::IntegrationUpdate(const float deltaTime)
{
	m_Acceleration = m_ThrustForce * m_Parameters.MassInverse; 
	if (!m_ImpulseForces.empty())
	{
		for (auto i = m_ImpulseForces.begin(); i < m_ImpulseForces.end(); i++)
		{
			m_Acceleration += (*i);// * m_Parameters.MassInverse;
		}
		m_ImpulseForces.clear();
	}

	vector3df k[4];

	k[0] = m_Acceleration;
	k[1] = m_Acceleration + ((m_Acceleration + k[0] * 0.5f) - m_Velocity * m_Parameters.Drag) * deltaTime * 0.5f;
	k[2] = m_Acceleration + ((m_Acceleration + k[1] * 0.5f) - m_Velocity * m_Parameters.Drag) * deltaTime * 0.5f;
	k[3] = m_Acceleration + ((m_Acceleration + k[2]) - m_Velocity * m_Parameters.Drag) * deltaTime;


	m_Velocity = m_Velocity + (k[0] + k[1] * 2.0f + k[2] * 2.0f + k[3]) * deltaTime * 0.1666666666666f;
	float velocityMagnitude = std::min(m_Velocity.getLength(), m_Parameters.MaxVelocity);
	m_Velocity.normalize() *= velocityMagnitude; 
	
	k[0] = m_Velocity;
	k[1] = m_Velocity + (m_Velocity + k[0] * 0.5f) * deltaTime * 0.5f;
	k[2] = m_Velocity + (m_Velocity + k[1] * 0.5f) * deltaTime * 0.5f;
	k[3] = m_Velocity + (m_Velocity + k[2]) * deltaTime;

	m_Position = m_Position + (k[0] + k[1] * 2.0f + k[2] * 2.0f + k[3]) * deltaTime * 0.1666666666666f;
}

void PhysicsObject::Update(const float deltaTime)
{
		
	m_RemainingLifespan -= deltaTime;
	
	if (!m_Parameters.HasLifespan || (m_Parameters.HasLifespan && m_RemainingLifespan > 0.0f))
	{
		IntegrationUpdate(deltaTime);
		if (m_Parameters.Listener) m_Parameters.Listener->Update(m_Position, m_Velocity);
	}
	else if (m_Parameters.HasLifespan && m_RemainingLifespan <= 0.0f)
	{
		MarkForCleanup();
		m_Parameters.Listener->OnDestroy();
	}

}

void PhysicsObject::ApplyThrust(const vector3df &direction, const float thrustVal)
{
	m_ThrustForce = direction * m_Parameters.LinearThrust * thrustVal; 
}

void PhysicsObject::ApplyImpulse(const vector3df &impulse)
{
	m_ImpulseForces.push_back(impulse);
}

void PhysicsObject::Reset(const vector3df& pos)
{
	m_ThrustForce = vector3df(0.0f, 0.0f, 0.0f);
	m_Position = pos; 
	m_Velocity = vector3df(0.0f, 0.0f, 0.0f);
	m_Acceleration = vector3df(0.0f, 0.0f, 0.0f);
}

void PhysicsObject::SetPosition(vector3df& pos)
{
	m_Position = pos; 
}