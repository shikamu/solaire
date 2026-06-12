#pragma once
#include "TargettingModule.h"
#include "Actuator.h"

class SpaceObject;
class LogicScene;

class AIController : public TargettingModule
{
private:
	float m_NewTargetTimer; 
	ActuatorData m_MasterData;

	ActuatorData m_ApproachData;
	ActuatorData m_ChaseData; 
	ActuatorData m_AttackData;
	ActuatorData m_EscapeData; 
	ActuatorData m_AvoidData;

	float m_ApproachWeight; 
	float m_ChaseWeight; 
	float m_AttackWeight; 
	float m_EscapeWeight; 
	float m_AvoidWeight;

	SpaceObject* m_CurrentTarget;

	void EvaluateTargets();
	void CalculateApproach();
	void CalculateChase(); 
	void CalculateAttack();
	void CalculateEscape();
	void CalculateAvoid(); 
	void CombineData(); 

	void UseModules(); 
public:
	AIController();
	AIController(SpaceObject* host, LogicScene* scene);
	~AIController();
	void Update(float dt);
	SpaceObject* GetCurrentTarget();
	ActuatorData& GetActuatorData(); 
};

