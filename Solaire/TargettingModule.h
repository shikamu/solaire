#pragma once
#include <vector>

using std::vector;

class SpaceObject;
class LogicScene;

class TargettingModule
{
private:
	float *m_Range; 
	float *m_Speed; 
	float *m_Accuracy;
	float m_ActivationTimer; 
	float m_HardLockTimer; 
	float m_hardTimer;

	float m_hardLockTimerFraction;
	unsigned int m_previousTarget, m_currentTarget;

protected:
	SpaceObject *m_Host; 
	LogicScene *m_ParentScene;
	vector<SpaceObject*> m_Targets; 

public:
	TargettingModule(): m_Host(NULL), m_ParentScene(NULL), m_ActivationTimer(0.0f), m_Range(NULL), m_Speed(NULL), m_Accuracy(NULL), m_HardLockTimer (0.0f), m_hardTimer(0.0f), m_hardLockTimerFraction(0.0f), m_previousTarget(0), m_currentTarget(0) {}
	TargettingModule(SpaceObject* host, LogicScene *scene);
	virtual ~TargettingModule() {} 

	void UpdateTargetList(float dt); 
	void NextSoftTarget();
	void PrevSoftTarget();
	void FrontSoftTarget();

	unsigned int getPreviousTarget() const
	{
		return m_previousTarget;
	}

	unsigned int getCurrentTarget() const
	{
		return m_currentTarget;
	}

	float getHardLockTimerFraction() const
	{
		return m_hardLockTimerFraction;
	}

	void setHardLockTimerFraction(const float f)
	{
		m_hardLockTimerFraction = f;
	}

	float getHardTimer() const
	{
		return m_hardTimer;
	}

	float getHardLockTimer() const
	{
		return m_HardLockTimer;
	}
};