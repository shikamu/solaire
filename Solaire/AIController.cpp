#include <cmath>
#include <irrlicht.h>
#include "AIController.h"
#include "SpaceObject.h"
#include "LogicScene.h"
#include "LogicConstants.h"
#include "PropertyState.h"
#include "RenderObjectIncludes.h"
#include "LockPointer.h"
#include "PhysicsObject.h"
#include "LogicModule.h"

using irr::scene::IMeshSceneNode;
using irr::core::vector3df;

AIController::AIController() : TargettingModule(), m_CurrentTarget (NULL), m_NewTargetTimer(0.0f), m_ApproachWeight(0.0f), m_ChaseWeight(0.0f), m_AttackWeight(0.0f), m_EscapeWeight(0.0f), m_advanced(false)
{
}

AIController::AIController(SpaceObject* host, LogicScene* scene) : TargettingModule(host, scene), m_CurrentTarget (NULL), m_NewTargetTimer(0.0f), m_ApproachWeight(0.0f), m_ChaseWeight(0.0f), m_AttackWeight(0.0f), m_EscapeWeight(0.0f), m_advanced(false)
{
}

AIController::~AIController()
{
}
SpaceObject* AIController::GetCurrentTarget()
{
	return m_CurrentTarget; 
}

ActuatorData& AIController::GetActuatorData()
{
	return m_MasterData;
}
void AIController::EvaluateTargets()
{
	if (!m_Host) return; 

	if (m_Targets.size() == 0)
	{
		m_CurrentTarget = NULL;
		return; 
	}
	else if (m_CurrentTarget)
	{
		bool CurrentIsValid = false; 
		for (auto i = m_Targets.begin(); i < m_Targets.end(); i++)
		{
			if ((*i)->ID == m_CurrentTarget->ID) CurrentIsValid = true; 
		}
		if (!CurrentIsValid) m_CurrentTarget = NULL; 
	}


	//Heuristic Function to Select Target
	//Factors
	//	Health
	//	Attacking Friendlies
	//	Weighting towards current target
	//Tie-Breakers
	//	Range
	//	Score (?)
	
	SpaceObject* Candidate = NULL;
	float CandidateValue = 0.0f; 

	for (auto i = m_Targets.begin(); i < m_Targets.end(); i++)
	{
		if ((((*i)->ObjectMask & m_Host->ObjectMask) & GROUP_FILTER) != 0) continue; 
		
		float CurrentValue = 0.0f; 
		
		// Evaluate Candidate

		if ((*i)->GetSoftTarget())
		{
			if ((((*i)->GetSoftTarget()->ObjectMask & m_Host->ObjectMask) & GROUP_FILTER) != 0)
			{
				// Candidate is targetting an ally 
				CurrentValue += 3.0f; 
				if ((*i)->GetSoftTarget()->ID == m_Host->ID) CurrentValue += 2.0f; 
			}
			else 
			{
				// Candidate is targetting an enemy
				CurrentValue -= 2.0f; 
			}
		}
		if (m_Host->GetSoftTarget())
		{
			// Weighting for Current Target
			if ((*i)->ID == m_Host->GetSoftTarget()->ID) CurrentValue += 2.0f; 
		}


		// Weighting for ships with existing damage (maximum +16 for "dead" target) 
		PropertyState* Shield = (*i)->GetPropertyByID(PROP_SHIELD);
		PropertyState* Armour = (*i)->GetPropertyByID(PROP_ARMOUR); 

		if (Shield && Armour)
		{
			CurrentValue += 4.0f * (1.0f - *Shield->CurrentFractionPointer()); 
			CurrentValue += 4.0f * (1.0f - *Armour->CurrentFractionPointer()); 
		}


		// Compare with current candidate

		if (Candidate == NULL)
		{
			Candidate = (*i);
			CandidateValue = CurrentValue;
		}
		else
		{
			if (CurrentValue > CandidateValue + 2.0f)
			{
				Candidate = (*i);
				CandidateValue = CurrentValue; 
			}
			else if (CurrentValue > CandidateValue - 0.5f) 
			{
				// Tie-Break Situation
				float CurrentRange = (m_Host->GetRenderObject()->GetPosition() - (*i)->GetRenderObject()->GetPosition()).getLength();
				float CandidateRange = (m_Host->GetRenderObject()->GetPosition() - Candidate->GetRenderObject()->GetPosition()).getLength();

				float Difference = std::abs(CurrentRange - CandidateRange);

				if (CurrentRange > CandidateRange && CurrentRange / Difference < 4.0f)
				{
					Candidate = (*i);
					CandidateValue = CurrentValue; 

				}
			}
		}
	}

	if (!m_Host->GetSoftTarget())
	{
		m_CurrentTarget = Candidate;
		m_Host->SetSoftTarget(Candidate);
	}
	else if (!Candidate || m_Host->GetSoftTarget()->ID != Candidate->ID)
	{
		//Candidate can be NULL when every remaining target is an ally (e.g. all enemies
		//left/died). Guard the ID dereference - without this the host crashes.
		m_CurrentTarget = Candidate;
		m_Host->SetSoftTarget(Candidate);
	}
}

void AIController::Update(float dt)
{
	UpdateTargetList(dt);
	m_NewTargetTimer += dt;
	//Advanced bots reassess their target much more often (more responsive/aggressive).
	if (m_NewTargetTimer > (m_advanced ? 0.6f : 2.0f))
	{
		EvaluateTargets();
		m_NewTargetTimer = 0.0f;
	}
	if (!m_CurrentTarget)
	{
		// Patrol Code 

		m_MasterData.ButtonMask = 0;
		m_MasterData.ThrustVal = 0.7f;
		m_MasterData.PitchVal = -0.2f;
		m_MasterData.RollVal = 0.05f;
		m_MasterData.YawVal = 0.0f; 

		return; 
	}

	CalculateApproach();
	CalculateEscape(); 
	CalculateChase(); 
	CalculateAttack();
	

	CombineData(); 
	UseModules();
	//m_MasterData = m_EvadeData;
}



void AIController::CalculateApproach()
{
	if (!m_CurrentTarget) return;
		
	irr::core::matrix4 Inverse;
	m_Host->GetRenderObject()->GetSceneNode()->getRelativeTransformation().getInverse(Inverse);
			
		
	vector3df EnemyPos = m_CurrentTarget->GetRenderObject()->GetPosition();
	Inverse.transformVect(EnemyPos);
	m_ApproachWeight = std::max(0.0f, EnemyPos.getLength() - 1500.0f); 
	EnemyPos.normalize();

	m_ApproachData.ThrustVal = (EnemyPos.dotProduct(vector3df(0.0f, 0.0f, 1.0f)) + 1.0f) * 0.5f;
	m_ApproachData.PitchVal = -EnemyPos.Y * (1.0f - m_ApproachData.ThrustVal * 0.3f);
	m_ApproachData.YawVal = EnemyPos.X * (1.0f - m_ApproachData.ThrustVal * 0.3f);

}

void AIController::CalculateChase()
{
	if (!m_CurrentTarget) return;

	vector3df TargetVelocity = m_CurrentTarget->GetVelocity();
	vector3df HostVelocity = m_Host->GetVelocity();

	irr::core::matrix4 Inverse;
	m_Host->GetRenderObject()->GetSceneNode()->getRelativeTransformation().getInverse(Inverse);
	Inverse.transformVect(TargetVelocity);
	Inverse.transformVect(HostVelocity);
	vector3df EnemyPos = m_CurrentTarget->GetRenderObject()->GetPosition();


	vector3df RequiredDirection = TargetVelocity - HostVelocity; 
	RequiredDirection.normalize(); 
	
	float ThrustFactor = pow(std::max(0.0f, RequiredDirection.dotProduct(vector3df(0.0f, 0.0f, 1.0f))), 3);
	m_ChaseData.ThrustVal = pow(std::max(0.0f, std::min(1.0f, ThrustFactor)), 3);
	m_ChaseData.PitchVal = -RequiredDirection.Y * (1.0f - m_ApproachData.ThrustVal * 0.3f);
	m_ChaseData.YawVal = RequiredDirection.X * (1.0f - m_ApproachData.ThrustVal * 0.3f);
	
	Inverse.transformVect(EnemyPos);
	m_ChaseWeight = std::max(0.0f, 2000.0f - EnemyPos.getLength());
	m_ChaseWeight *= pow(1.0f - (HostVelocity.dotProduct(TargetVelocity) * 0.5f + 0.5f), 2);

}
void AIController::CalculateAttack()
{
	if (!m_CurrentTarget) return;
	
	irr::core::matrix4 Inverse;
	m_Host->GetRenderObject()->GetSceneNode()->getRelativeTransformation().getInverse(Inverse);
			
		
	vector3df EnemyPos = m_CurrentTarget->GetRenderObject()->GetPosition() + m_CurrentTarget->GetVelocity() * 0.5f;
	vector3df RelativeVelocity = m_CurrentTarget->GetVelocity() - m_Host->GetVelocity();
	
	Inverse.transformVect(EnemyPos);

	m_AttackWeight = std::max(0.0f, 3800.0f - EnemyPos.getLength() - RelativeVelocity.getLength());

	EnemyPos.normalize();

	m_AttackData.ThrustVal = 0.0f;
	m_AttackData.PitchVal = -EnemyPos.Y;
	m_AttackData.YawVal = EnemyPos.X;
	
	vector3df EnemyDirection = m_CurrentTarget->GetRenderObject()->GetRotation().rotationToDirection();
	Inverse.transformVect(EnemyDirection);
	EnemyDirection.normalize(); 
	
	//m_AttackWeight *= 100.0f * pow((m_Host->GetVelocity().dotProduct(m_CurrentTarget->GetVelocity()) * 0.5f + 0.5f), 2);

	if (m_AttackWeight > 200.0f)
	{
		m_ChaseWeight = 0.0f;
		m_AttackWeight *= 100.0f;
	}

}
void AIController::CalculateEscape()
{
	if (!m_CurrentTarget) return;

	irr::core::matrix4 Inverse;
	m_Host->GetRenderObject()->GetSceneNode()->getRelativeTransformation().getInverse(Inverse);
		
	vector3df EnemyPos = -m_CurrentTarget->GetRenderObject()->GetPosition();
	Inverse.transformVect(EnemyPos);
	EnemyPos.normalize();

	m_EscapeData.ThrustVal = (EnemyPos.dotProduct(vector3df(0.0f, 0.0f, 1.0f)) + 1.0f) * 0.5f;
	m_EscapeData.PitchVal = -EnemyPos.Y * (1.0f - m_EscapeData.ThrustVal * 0.6f);
	m_EscapeData.YawVal = EnemyPos.X * (1.0f - m_EscapeData.ThrustVal * 0.6f);	

	PropertyState* Shield = m_Host->GetPropertyByID(PROP_SHIELD);
	if (Shield)
	{
		m_EscapeWeight = std::pow((1.0f - *Shield->CurrentFractionPointer()),4) * 1000.0f; 
	}
}

void AIController::CalculateAvoid()
{
	SpaceObject* NearestTarget = NULL;
	float NearesetDistance = 1000.0f;

	for (auto i = m_Targets.begin(); i < m_Targets.end(); i++)
	{
		float TargetDistance = (m_Host->GetRenderObject()->GetPosition() - (*i)->GetRenderObject()->GetPosition()).getLength();
		if (TargetDistance < NearesetDistance)
		{
			NearestTarget = (*i);
			NearesetDistance = TargetDistance;
		}
	}	
	
	irr::core::matrix4 Inverse;
	m_Host->GetRenderObject()->GetSceneNode()->getRelativeTransformation().getInverse(Inverse);
		
	vector3df EnemyPos = -NearestTarget->GetRenderObject()->GetPosition();
	Inverse.transformVect(EnemyPos);
	EnemyPos.normalize();

	m_AvoidData.ThrustVal = (EnemyPos.dotProduct(vector3df(0.0f, 0.0f, 1.0f)) + 1.0f) * 0.5f;
	m_AvoidData.PitchVal = -EnemyPos.Y * (1.0f - m_AvoidData.ThrustVal * 0.6f);
	m_AvoidData.YawVal = EnemyPos.X * (1.0f - m_AvoidData.ThrustVal * 0.6f);	

	m_AvoidWeight = std::pow(std::max(0.0f, 600.0f - NearesetDistance), 3); 
}


void AIController::CombineData()
{
	float TotalWeight =			m_ApproachWeight + 
								m_ChaseWeight + 
								m_AttackWeight + 
								m_EscapeWeight + 
								m_AvoidWeight;

	m_MasterData.PitchVal =		m_ApproachData.PitchVal *	m_ApproachWeight +
								m_ChaseData.PitchVal *		m_ChaseWeight +
								m_AttackData.PitchVal *		m_AttackWeight + 
								m_EscapeData.PitchVal *		m_EscapeWeight +
								m_AvoidData.PitchVal *		m_AvoidWeight;

	m_MasterData.PitchVal /= TotalWeight;

	m_MasterData.YawVal =		m_ApproachData.YawVal *		m_ApproachWeight +
								m_ChaseData.YawVal *		m_ChaseWeight +
								m_AttackData.YawVal *		m_AttackWeight + 
								m_EscapeData.YawVal *		m_EscapeWeight+
								m_AvoidData.YawVal *		m_AvoidWeight;

	m_MasterData.YawVal /= TotalWeight;

	m_MasterData.ThrustVal =	m_ApproachData.ThrustVal *	m_ApproachWeight +
								m_ChaseData.ThrustVal *		m_ChaseWeight +
								m_AttackData.ThrustVal *	m_AttackWeight + 
								m_EscapeData.ThrustVal *	m_EscapeWeight+
								m_AvoidData.ThrustVal *		m_AvoidWeight;

	m_MasterData.ThrustVal /= TotalWeight;

	m_MasterData.ButtonMask = m_AttackData.ButtonMask;
}

void AIController::UseModules()
{
	for (auto i = m_Host->Modules.begin(); i < m_Host->Modules.end(); i++)
	{
		if ((*i)->ActivationType == ACT_COLLISION) continue; 
		switch ((*i)->Type)
		{
		case MT_OFFENSIVE:
			{
				if (!m_Host->GetSoftTarget()) continue;
				irr::core::matrix4 Inverse;
				m_Host->GetRenderObject()->GetSceneNode()->getRelativeTransformation().getInverse(Inverse);
				vector3df EnemyPos = m_CurrentTarget->GetRenderObject()->GetPosition();
				Inverse.transformVect(EnemyPos);
				EnemyPos.normalize();

				//Advanced bots open fire from a wider aim cone and a longer range, so they
				//land hits more often.
				const float aimThreshold = m_advanced ? 0.80f : 0.85f;
				const float weightThreshold = m_advanced ? 400.0f : 1000.0f;
				if (EnemyPos.dotProduct(vector3df(0.0f, 0.0f, 1.0f)) > aimThreshold && m_AttackWeight > weightThreshold)
						if ((*i)->FittedModule->Activate()) return;
			}
			break;
		case MT_DEFENSIVE:
			{
				PropertyState* Shield = m_Host->GetPropertyByID(PROP_SHIELD);
				PropertyState* Armour = m_Host->GetPropertyByID(PROP_ARMOUR);
				if (Shield && Armour)
				{
					if ((*Armour->CurrentFractionPointer() < 0.8f && *Shield->CurrentFractionPointer() < 0.7f) ||
						*Shield->CurrentFractionPointer() < 0.4f)
						if ((*i)->FittedModule->Activate()) return; 
				}
				else if (Shield)
				{
					if (*Shield->CurrentFractionPointer() < 0.4f) 
						if ((*i)->FittedModule->Activate()) return; 
				}
			}
			break;
		case MT_UTILITY:

			break; 
		}


	}
}