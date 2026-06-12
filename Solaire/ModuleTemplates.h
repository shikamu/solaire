#pragma once
#include "LogicModule.h"

class BulletDamageModule : public LogicModule
{
private:
	float m_ShieldDmg;
	float m_ArmourDmg;
public:
	BulletDamageModule() : m_ShieldDmg (1.0f), m_ArmourDmg (1.0f) 
	{
	}
	BulletDamageModule(float shield, float armour) : m_ShieldDmg (shield), m_ArmourDmg (armour) 
	{
	}
	~BulletDamageModule() {} 
	bool Activate(SpaceObject* target);
	bool Activate();
};

class BulletEMPModule : public LogicModule
{
private:
	float m_ThrustMod;
	float m_ManMod;
public:
	BulletEMPModule() : m_ThrustMod (0.0f), m_ManMod (0.0f) 
	{
	}
	BulletEMPModule(float thrust, float man) : m_ThrustMod (thrust), m_ManMod (man) 
	{
	}
	~BulletEMPModule() {} 
	bool Activate(SpaceObject* target);
	bool Activate();
};

class MachineGunModule : public LogicModule
{
private:
public:
	MachineGunModule() 
	{
		m_Cooldown = 0.1f;
	}
	MachineGunModule(float cooldown) 
	{
		m_Cooldown = cooldown;
	}
	~MachineGunModule() {} 
	bool Activate(SpaceObject* target);
	bool Activate();
	void Update(float dT) 
	{
		m_CurrentCooldown += dT;
	} 
};

class DevRepairModule : public LogicModule
{
private:
public:
	DevRepairModule()
	{
		m_Cooldown = 1.0f; 
	}
	~DevRepairModule() {} 
	bool Activate(SpaceObject* target);
	bool Activate();
	void Update(float dT) 
	{
		m_CurrentCooldown += dT;
	} 
};

class ShieldRegenModule : public LogicModule
{
private:
public:
	ShieldRegenModule()
	{
		m_Cooldown = 0.5f; 
	}
	~ShieldRegenModule() {} 
	bool Activate(SpaceObject* target);
	bool Activate();
	void Update(float dT) 
	{
		m_CurrentCooldown += dT;
	} 
};

class RocketLauncherModule : public LogicModule
{
private:
	float m_SubCooldown; 
	int m_FireCount;
	bool Launch();
public:
	RocketLauncherModule() : m_SubCooldown (0.2f), m_FireCount(0)
	{
		m_Cooldown = 2.0f;
	}
	RocketLauncherModule(float cooldown) 
	{
		m_Cooldown = cooldown;
	}
	~RocketLauncherModule() {} 
	bool Activate(SpaceObject* target);
	bool Activate();
	void Update(float dT) 
	{
		m_CurrentCooldown += dT;
		m_SubCooldown += dT;
	} 
};

class TorpedoLauncherModule : public LogicModule
{
private:
public:
	TorpedoLauncherModule() 
	{
		m_Cooldown = 2.0f;
	}
	TorpedoLauncherModule(float cooldown) 
	{
		m_Cooldown = cooldown;
	}
	~TorpedoLauncherModule() {} 
	bool Activate(SpaceObject* target);
	bool Activate();
	void Update(float dT) 
	{
		m_CurrentCooldown += dT;
	} 
};