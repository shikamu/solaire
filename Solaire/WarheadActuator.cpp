#include <vector3d.h>
#include <irrlicht.h>
#include <cmath>
#include "WarheadActuator.h" 
#include "SpaceObject.h"
#include "PropertyState.h"
#include "LogicConstants.h"
#include "RenderObjectIncludes.h" 

using irr::scene::IMeshSceneNode;
using namespace irr;

void WarheadActuator::update(SpaceObject* host, const float dt)
{
	m_Fuse -= dt; 
	if (m_Fuse > 0.0f)
	{
		m_Thrust = 1.0f;
		return;
	}
	float RotationFactor = 0.1f;
	if (host)
	{
		PropertyState* HostMan = host->GetPropertyByID(PROP_MANEUVERABILITY);
		PropertyState* HostThrust = host->GetPropertyByID(PROP_THRUST);
		if (HostMan)
		{
			RotationFactor = HostMan->Current() ; 
		}
		if (HostThrust)
		{
			m_Thrust *= HostThrust->Current();
		}
		if (host->GetHardTarget())
		{
			irr::core::matrix4 Inverse;
			host->GetRenderObject()->GetSceneNode()->getRelativeTransformation().getInverse(Inverse);
			
		
			vector3df EnemyPos = host->GetHardTarget()->GetRenderObject()->GetPosition();
			Inverse.transformVect(EnemyPos);
			EnemyPos.normalize();

			m_Thrust = std::pow(std::max(0.0f, EnemyPos.dotProduct(vector3df(0.0f, 0.0f, 1.0f))), 25);
			m_FPitch = -EnemyPos.Y * (1.0f - m_Thrust * 0.6f);
			m_FYaw = EnemyPos.X * (1.0f - m_Thrust * 0.6f);
		}
		else
		{
			m_FRoll = 0.0f * (1.0f - m_Thrust * 0.8f);
			m_FPitch = 0.0f * (1.0f - m_Thrust * 0.8f); 
			m_FYaw = -0.5f * (1.0f - m_Thrust * 0.8f); 
		}

	}

	m_QRoll.fromAngleAxis(m_FRoll * RotationFactor * dt, core::vector3df(0, 0, 1)); 
	m_QRoll.normalize();

	m_QPitch.fromAngleAxis(m_FPitch * RotationFactor * dt, core::vector3df(1, 0, 0));
	m_QPitch.normalize();

	m_QYaw.fromAngleAxis(m_FYaw * RotationFactor * dt, core::vector3df(0, 1, 0));
	m_QYaw.normalize();

	core::quaternion combination = m_QRoll*m_QPitch*m_QYaw;
	combination.normalize();
	m_QAll = combination; 
	m_QCurrent *= m_QAll;
	m_QCurrent.toEuler(m_Rot);
	m_Rot *= core::RADTODEG;
	m_Direction = m_Rot.rotationToDirection(core::vector3df(0, 0, 1));
	m_ButtonMask |= INPUT_PRIMARY;
	m_Firing = true;
	m_Secondary = false;
	m_Tertiary = false;
}