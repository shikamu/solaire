#include <cassert>

#include "SpaceObjectFactory.h"
#include "PhysicsController.h"
#include "System.h"
#include "SpaceObject.h"
#include "LogicModule.h"
#include "LogicScene.h"
#include "RenderObjectIncludes.h"
#include "ModuleTemplates.h"
#include "PhysicsParams.h"
#include "Actuator.h"
#include "HumanActuator.h"
#include "DummyActuator.h"
#include "TargettingModule.h"
#include "LogicConstants.h"
#include "LockPointer.h"
#include "SpaceObjectShell.h"
#include "NetworkController.h"
#include "LANServer.h"
#include "Agent.h"
#include "ClientHumanActuator.h"
#include "NetworkActuator.h"
#include "WarheadActuator.h"
#include "AIActuator.h"

SpaceObjectFactory SpaceObjectFactory::m_Factory;

SpaceObjectFactory& SpaceObjectFactory::Get()
{
	return m_Factory;
}

unsigned int SpaceObjectFactory::CreateProjectile(LogicScene* parentScene, SpaceObject* hostObj, const PROJECTILE_TYPE type, float* values, const int valueCount)
{
	SpaceObject* obj = new SpaceObject(parentScene); 
	unsigned int ID = parentScene->RequestSpaceObjectID();
	obj->ID = ID;
	unsigned int mask = (hostObj->ObjectMask & (MASK_GROUP_1 | MASK_GROUP_2 | MASK_GROUP_3 | MASK_GROUP_4)) | MASK_PROJECTILE;
	obj->ObjectMask = mask;	
	obj->SetParentScene(parentScene);

	ProjectileRenderObject* ro = new ProjectileRenderObject();
	ro->Init(parentScene->getSceneManager(), obj, hostObj, type);
	obj->SetRenderObject(ro);
	
	ModuleNode* ModNode = new ModuleNode();
	PhysicsParams physParams;

	switch (type)
	{
	case PROJECTILE_EMP:
	case PROJECTILE_SLUG:
	case PROJECTILE_FLECHETTE:
	case PROJECTILE_BULLET:

			if (valueCount!= 2) return 0; 
			ModNode->Size = MS_ZERO;
			ModNode->Type = MT_OFFENSIVE;
			ModNode->ActivationType = ACT_COLLISION;
			obj->m_ModuleListLock.Lock();
			obj->Modules.push_back(ModNode);
			obj->Modules.back()->FittedModule = new BulletDamageModule(values[0], values[1]);
			obj->Modules.back()->FittedModule->SetActType(ACT_COLLISION);
			obj->Modules.back()->FittedModule->SetSource(hostObj);
			obj->m_ModuleListLock.Unlock();

			
			physParams.LinearThrust = 10000.0f;
			physParams.Drag = 0.0f;
			physParams.Mass = 0.1f;
			physParams.MaxVelocity = 100000000000.0f;
			physParams.InitialPosition = hostObj->GetRenderObject()->GetPosition();
			physParams.InitialVelocity = hostObj->GetRenderObject()->GetRotation().rotationToDirection() * 10000.0f;
			physParams.HasLifespan = true;
			physParams.IsVolatile = true;
			physParams.Lifespan = 0.5f;
			physParams.Listener = obj;
			physParams.Size = 10.0f;
			physParams.ID = ID; 
			physParams.ParentID = hostObj->ID;
			physParams.ObjectMask = mask;
		break;
	default:
		break;
	}

	
	obj->SetPhysicsObject(PhysicsController::Get().CreateObject(physParams));
	parentScene->AddSpaceObject(ID, obj);

	if (parentScene->IsNetworked())
	{
		SpaceObjectShell* newShell = new SpaceObjectShell();
		newShell->Position = hostObj->GetRenderObject()->GetPosition();
		newShell->Rotation = hostObj->GetRenderObject()->GetRotation();
		newShell->Mask = obj->ObjectMask; 
		newShell->ID = ID; 
		newShell->SubTypeID = type;
		newShell->ServerSideObj = obj;
		LockPointer<SpaceObjectShell>* LP = new LockPointer<SpaceObjectShell>(newShell);
		obj->SetShell(LP);
		NetworkController::get().getServer()->addShell(LP);
	}


	return ID;
}
unsigned int SpaceObjectFactory::CreateWarhead(LogicScene* parentScene, SpaceObject* hostObj, const WARHEAD_TYPE type, float* values, const int valueCount)
{
	SpaceObject* obj = new SpaceObject(parentScene); 
	unsigned int ID = parentScene->RequestSpaceObjectID();
	obj->ID = ID;
	obj->SetAgentID(hostObj->GetAgentID());
	unsigned int mask = (hostObj->ObjectMask & (MASK_GROUP_1 | MASK_GROUP_2 | MASK_GROUP_3 | MASK_GROUP_4)) | MASK_WARHEAD;
	obj->ObjectMask = mask;	
	obj->SetParentScene(parentScene);

	WarheadRenderObject* ro = new WarheadRenderObject();
	ro->Init(parentScene->getSceneManager(), obj, hostObj, type);
	obj->SetRenderObject(ro);
	
	PhysicsParams physParams;
	ModuleNode* ModNode = new ModuleNode();

	switch (type)
	{
	case WARHEAD_MISSILE:
	case WARHEAD_EMP:
		{
			if (valueCount!= 2) return 0; 
			ModNode->Size = MS_ZERO;
			ModNode->Type = MT_OFFENSIVE;
			ModNode->ActivationType = ACT_COLLISION;
			obj->m_ModuleListLock.Lock();
			obj->Modules.push_back(ModNode);
			obj->Modules.back()->FittedModule = new BulletEMPModule(values[0], values[1]);
			//else if (type == WARHEAD_EMP)
			//	obj->Modules.back()->FittedModule = new BulletEMPModule(values[0], values[1]);
			obj->Modules.back()->FittedModule->SetActType(ACT_COLLISION);
			obj->Modules.back()->FittedModule->SetSource(hostObj);
			obj->m_ModuleListLock.Unlock();

			PropertyState EngineThrust(1.0f, 0.0f, 1.0f, PROP_THRUST, L"EngineThrust");
			obj->AddPropertyState(PROP_THRUST, EngineThrust); 

			PropertyState EngineMan(20.0f, 0.0f, 20.0f, PROP_MANEUVERABILITY, L"EngineMan"); // Change to 50 for photon torpedo
			obj->AddPropertyState(PROP_MANEUVERABILITY, EngineMan); 
	
			obj->SetActuator(new WarheadActuator(1.0f));
			obj->GetActuator()->create(hostObj->GetRenderObject()->GetSceneNode(), parentScene->getSceneManager());

			physParams.LinearThrust = 100000.0f;
			physParams.Drag = 100000.0f;
			physParams.Mass = 1.0f;
			physParams.MaxVelocity = 100000000.0f;
			physParams.InitialPosition = hostObj->GetRenderObject()->GetPosition() + hostObj->GetRenderObject()->GetRotation().rotationToDirection(vector3df(0.0f, -1.0f, 0.0f)) * 40.0f;
			//physParams.InitialVelocity = hostObj->GetPhysicsObject()->GetPointer()->GetVelocity() + hostObj->GetRenderObject()->GetRotation().rotationToDirection() * hostObj->GetPhysicsObject()->GetPointer()->GetVelocity().getLength();
			//hostObj->GetPhysicsObject()->Unlock();
			//hostObj->GetPhysicsObject()->Unlock();

			//physParams.InitialVelocity = vector3df(0.0f, 0.0f, 0.0f);

			physParams.HasLifespan = true;
			physParams.IsVolatile = true;
			physParams.Lifespan = 7.0f;
			physParams.Listener = obj;
			physParams.Size = 10.0f;
			physParams.ID = ID; 
			physParams.ParentID = hostObj->ID;
			physParams.ObjectMask = mask;
		}
		break;
	case WARHEAD_ROCKET:
		{
			if (valueCount!= 2) return 0; 
			ModNode->Size = MS_ZERO;
			ModNode->Type = MT_OFFENSIVE;
			ModNode->ActivationType = ACT_COLLISION;
			obj->m_ModuleListLock.Lock();
			obj->Modules.push_back(ModNode);
			obj->Modules.back()->FittedModule = new BulletDamageModule(values[0], values[1]);
			//else if (type == WARHEAD_EMP)
			//	obj->Modules.back()->FittedModule = new BulletEMPModule(values[0], values[1]);
			obj->Modules.back()->FittedModule->SetActType(ACT_COLLISION);
			obj->Modules.back()->FittedModule->SetSource(hostObj);
			obj->m_ModuleListLock.Unlock();

			PropertyState EngineThrust(1.0f, 0.0f, 1.0f, PROP_THRUST, L"EngineThrust");
			obj->AddPropertyState(PROP_THRUST, EngineThrust); 

			PropertyState EngineMan(20.0f, 0.0f, 20.0f, PROP_MANEUVERABILITY, L"EngineMan"); // Change to 50 for photon torpedo
			obj->AddPropertyState(PROP_MANEUVERABILITY, EngineMan); 
	
			obj->SetActuator(new WarheadActuator(1.0f));
			obj->GetActuator()->create(hostObj->GetRenderObject()->GetSceneNode(), parentScene->getSceneManager());

			physParams.LinearThrust = 10000.0f;
			physParams.Drag = 10000.0f;
			physParams.Mass = 1.0f;
			physParams.MaxVelocity = 100000000.0f;
			physParams.InitialPosition = hostObj->GetRenderObject()->GetPosition() + hostObj->GetRenderObject()->GetRotation().rotationToDirection(vector3df(0.0f, -1.0f, 0.0f)) * 40.0f;
			//physParams.InitialVelocity = hostObj->GetPhysicsObject()->GetPointer()->GetVelocity() + hostObj->GetRenderObject()->GetRotation().rotationToDirection() * hostObj->GetPhysicsObject()->GetPointer()->GetVelocity().getLength();
			//hostObj->GetPhysicsObject()->Unlock();
			//hostObj->GetPhysicsObject()->Unlock();

			//physParams.InitialVelocity = vector3df(0.0f, 0.0f, 0.0f);

			physParams.HasLifespan = true;
			physParams.IsVolatile = true;
			physParams.Lifespan = 10.0f;
			physParams.Listener = obj;
			physParams.Size = 10.0f;
			physParams.ID = ID; 
			physParams.ParentID = hostObj->ID;
			physParams.ObjectMask = mask;
		}
		break;
	case WARHEAD_TORPEDO:
		{
			if (valueCount!= 2) return 0; 
			ModNode->Size = MS_ZERO;
			ModNode->Type = MT_OFFENSIVE;
			ModNode->ActivationType = ACT_COLLISION;
			obj->m_ModuleListLock.Lock();
			obj->Modules.push_back(ModNode);
			obj->Modules.back()->FittedModule = new BulletDamageModule(values[0], values[1]);
			//else if (type == WARHEAD_EMP)
			//	obj->Modules.back()->FittedModule = new BulletEMPModule(values[0], values[1]);
			obj->Modules.back()->FittedModule->SetActType(ACT_COLLISION);
			obj->Modules.back()->FittedModule->SetSource(hostObj);
			obj->m_ModuleListLock.Unlock();

			PropertyState EngineThrust(1.0f, 0.0f, 1.0f, PROP_THRUST, L"EngineThrust");
			obj->AddPropertyState(PROP_THRUST, EngineThrust); 

			PropertyState EngineMan(5.0f, 0.0f, 5.0f, PROP_MANEUVERABILITY, L"EngineMan"); // Change to 50 for photon torpedo
			obj->AddPropertyState(PROP_MANEUVERABILITY, EngineMan); 
	
			obj->SetActuator(new WarheadActuator(3.0f));
			obj->GetActuator()->create(hostObj->GetRenderObject()->GetSceneNode(), parentScene->getSceneManager());

			physParams.LinearThrust = 10000.0f;
			physParams.Drag = 1000.0f;
			physParams.Mass = 10.0f;
			physParams.MaxVelocity = 100000000.0f;
			physParams.InitialPosition = hostObj->GetRenderObject()->GetPosition() + hostObj->GetRenderObject()->GetRotation().rotationToDirection(vector3df(0.0f, -1.0f, 0.0f)) * 40.0f;
			//physParams.InitialVelocity = hostObj->GetPhysicsObject()->GetPointer()->GetVelocity() + hostObj->GetRenderObject()->GetRotation().rotationToDirection() * hostObj->GetPhysicsObject()->GetPointer()->GetVelocity().getLength();
			//hostObj->GetPhysicsObject()->Unlock();
			//hostObj->GetPhysicsObject()->Unlock();

			//physParams.InitialVelocity = vector3df(0.0f, 0.0f, 0.0f);

			physParams.HasLifespan = true;
			physParams.IsVolatile = true;
			physParams.Lifespan = 20.0f;
			physParams.Listener = obj;
			physParams.Size = 10.0f;
			physParams.ID = ID; 
			physParams.ParentID = hostObj->ID;
			physParams.ObjectMask = mask;
		}
		break;

	//case WARHEAD_TURRET:
	//	{
	//		if (valueCount!= 2) return 0; 
	//		ModNode->Size = MS_ZERO;
	//		ModNode->Type = MT_OFFENSIVE;
	//		ModNode->ActivationType = ACT_PRIMARY;
	//		obj->m_ModuleListLock.Lock();
	//		obj->Modules.push_back(ModNode);
	//		obj->Modules.back()->FittedModule = new MachineGunModule(0.2f);
	//		obj->Modules.back()->FittedModule->SetActType(ACT_PRIMARY);
	//		obj->Modules.back()->FittedModule->SetSource(obj);
	//		obj->m_ModuleListLock.Unlock();
	//		obj->SetHardTarget(hostObj->GetHardTarget());

	//		PropertyState EngineThrust(0.0f, 0.0f, 0.0f, PROP_THRUST, L"EngineThrust");
	//		obj->AddPropertyState(PROP_THRUST, EngineThrust); 

	//		PropertyState EngineMan(50.0f, 0.0f, 50.0f, PROP_MANEUVERABILITY, L"EngineMan"); // Change to 50 for photon torpedo
	//		obj->AddPropertyState(PROP_MANEUVERABILITY, EngineMan); 
	//
	//		obj->SetActuator(new WarheadActuator(0.5f));
	//		obj->GetActuator()->create(hostObj->GetRenderObject()->GetSceneNode(), parentScene->getSceneManager());

	//		physParams.LinearThrust = 0.0f;
	//		physParams.Drag = 450.0f;
	//		physParams.Mass = 1.0f;
	//		physParams.MaxVelocity = 100000000.0f;
	//		physParams.InitialPosition = hostObj->GetRenderObject()->GetPosition();
	//		physParams.InitialVelocity = vector3df(0.0f, 0.0f, 0.0f);
	//		physParams.HasLifespan = true;
	//		physParams.IsVolatile = true;
	//		physParams.Lifespan = 20.0f;
	//		physParams.Listener = obj;
	//		physParams.Size = 10.0f;
	//		physParams.ID = ID; 
	//		physParams.ParentID = hostObj->ID;
	//		physParams.ObjectMask = mask;
	//	}
	//	break; 
	default:
		break;
	}

	
	obj->SetPhysicsObject(PhysicsController::Get().CreateObject(physParams));
	parentScene->AddSpaceObject(ID, obj);

	if (parentScene->IsNetworked())
	{
		SpaceObjectShell* newShell = new SpaceObjectShell();
		newShell->Position = hostObj->GetRenderObject()->GetPosition();
		newShell->Rotation = hostObj->GetRenderObject()->GetRotation();
		newShell->Mask = obj->ObjectMask; 
		newShell->ID = ID; 
		newShell->SubTypeID = type;
		newShell->ServerSideObj = obj;
		LockPointer<SpaceObjectShell>* LP = new LockPointer<SpaceObjectShell>(newShell);
		obj->SetShell(LP);
		NetworkController::get().getServer()->addShell(LP);
	}
	return ID;
}

unsigned int SpaceObjectFactory::CreateShip(LogicScene* parentScene, ACTUATOR_TYPE actType, unsigned int agentID, stringw name, const unsigned int mask, vector3df& pos, vector3df& rot, const bool needsCreation)
{
	
	SpaceObject* obj = new SpaceObject(parentScene); 
	unsigned int ID = parentScene->RequestSpaceObjectID();
	obj->ID = ID;
	obj->SetAgentID(agentID);
	obj->ObjectMask = (mask & (MASK_GROUP_1 | MASK_GROUP_2 | MASK_GROUP_3 | MASK_GROUP_4)) | MASK_SHIP;

	ShipRenderObject* sro = new ShipRenderObject();
	sro->Init(parentScene->getSceneManager(), obj);
	sro->SetPosition(pos);
	sro->SetRotation(rot);

	obj->SetRenderObject(sro);
	obj->SetParentScene(parentScene);
		
	ModuleNode* ModNode1 = new ModuleNode();
	ModNode1->Size = MS_SMALL;
	ModNode1->Type = MT_OFFENSIVE;
	ModNode1->ActivationType = ACT_PRIMARY;
				
	ModuleNode* ModNode2 = new ModuleNode();
	ModNode2->Size = MS_ZERO;
	ModNode2->Type = MT_DEFENSIVE;
	ModNode2->ActivationType = ACT_SECONDARY;
	
	ModuleNode* ModNode3 = new ModuleNode();
	ModNode3->Size = MS_SMALL;
	ModNode3->Type = MT_UTILITY;
	ModNode3->ActivationType = ACT_TERTIARY;

	obj->m_ModuleListLock.Lock();

	obj->Modules.push_back(ModNode1);
	obj->Modules.back()->FittedModule = new MachineGunModule(0.1f);
	obj->Modules.back()->FittedModule->SetActType(ACT_PRIMARY);
	obj->Modules.back()->FittedModule->SetSource(obj);

	// TODO : Fix Copy constructor for the module nodes

	obj->Modules.push_back(ModNode2);
	obj->Modules.back()->FittedModule = new ShieldRegenModule();
	obj->Modules.back()->FittedModule->SetActType(ACT_SECONDARY);
	obj->Modules.back()->FittedModule->SetSource(obj);
	
	obj->Modules.push_back(ModNode3);
	obj->Modules.back()->FittedModule = new RocketLauncherModule(2.5f);
	obj->Modules.back()->FittedModule->SetActType(ACT_TERTIARY);
	obj->Modules.back()->FittedModule->SetSource(obj);

	obj->m_ModuleListLock.Unlock();
	
	PropertyState Shield(100.0f, 0.0f, 100.0f, PROP_SHIELD, L"Shield");
	obj->AddPropertyState(PROP_SHIELD, Shield);
	PropertyState Armour(100.0f, 0.0f, 100.0f, PROP_ARMOUR, L"Armour");
	obj->AddPropertyState(PROP_ARMOUR, Armour);
	PropertyState ShieldInt(1.0f, 0.0f, 1.0f, PROP_SHIELD_INTEGRITY, L"ShieldIntegrity");
	obj->AddPropertyState(PROP_SHIELD_INTEGRITY, ShieldInt);

	PropertyState SensorRange(50000.0f, 0.0f, 50000.0f, PROP_SENSOR_RANGE, L"SensorRange");
	obj->AddPropertyState(PROP_SENSOR_RANGE, SensorRange); 
	PropertyState SensorSpeed(10.0f, 0.1f, 4.0f, PROP_SENSOR_SPEED, L"SensorSpeed");
	obj->AddPropertyState(PROP_SENSOR_SPEED, SensorSpeed); 
	PropertyState SensorAccuracy(1.0f, 0.1f, 0.1f, PROP_SENSOR_ACCURACY, L"SensorAccuracy");
	obj->AddPropertyState(PROP_SENSOR_ACCURACY, SensorAccuracy); 
	
	PropertyState EngineThrust(1.0f, 0.0f, 1.0f, PROP_THRUST, L"EngineThrust");
	obj->AddPropertyState(PROP_THRUST, EngineThrust); 

	PropertyState EngineMan(5.0f, 0.0f, 3.0f, PROP_MANEUVERABILITY, L"EngineMan");
	obj->AddPropertyState(PROP_MANEUVERABILITY, EngineMan); 

	switch (actType)
	{
		case ACT_LOCAL:
		{
			obj->SetActuator(new HumanActuator());
			obj->GetActuator()->create(sro->GetSceneNode(), parentScene->getSceneManager());
			break;
		}
		case ACT_AI_BASIC:
		{
			obj->SetActuator(new AIActuator());
			obj->GetActuator()->create(sro->GetSceneNode(), parentScene->getSceneManager());
			break;
		}
		case ACT_AI_ADV:
		{
			break;
		}
		case ACT_NET_CLIENT:
		{
			// If this is entered SOMETHING WENT WRONG!!11eleven!!
			assert(0);
			break;
		}
		case ACT_NET_SERVER:
		{
			obj->SetActuator(new NetworkActuator());
			obj->GetActuator()->create(sro->GetSceneNode(), parentScene->getSceneManager());	
			break;
		}
		default:
			assert(0);
			break;
	}
	//if (hostAgent->GetActuator())
	//{
	//	obj->SetActuator(hostAgent->GetActuator());
	//	hostAgent->GetActuator()->create(sro->GetSceneNode(), parentScene->getSceneManager());
	//}

	PhysicsParams physParams;
	physParams.LinearThrust = 100000.0f;
	physParams.Drag = 10.0f;
	physParams.Mass = 100.0f;
	physParams.Listener = obj;
	physParams.Size = 100.0f;
	physParams.ID = ID;
	physParams.ObjectMask = obj->ObjectMask;
	physParams.InitialPosition = pos;
	obj->PLSensorRange(200.f);

	obj->SetPhysicsObject(PhysicsController::Get().CreateObject(physParams));
	obj->SetTargetter(new TargettingModule(obj, parentScene));

	parentScene->AddSpaceObject(ID, obj);

	if (parentScene->IsNetworked())
	{
		SpaceObjectShell* newShell = new SpaceObjectShell();
		newShell->Position = pos;
		newShell->Rotation = pos;
		newShell->Mask = obj->ObjectMask; 
		newShell->ID = ID; 
		newShell->SubTypeID = 1;
		newShell->AgentID = agentID;
		newShell->Name = name;
		newShell->TargetID = 0; 
		newShell->ServerSideObj = obj;
		newShell->NeedsCreation = needsCreation;

		LockPointer<SpaceObjectShell>* LP = new LockPointer<SpaceObjectShell>(newShell);
		obj->SetShell(LP);
		NetworkController::get().getServer()->addShell(LP);
	}

	return ID;
}

unsigned int SpaceObjectFactory::CreateObjectFromShell(LogicScene* parentScene, SpaceObjectShell* shell)
{
	SpaceObject* obj = new SpaceObject(parentScene); 
	obj->SetName(shell->Name);
	obj->SetAgentID(shell->AgentID);
	obj->ID = shell->ID;
	obj->ObjectMask = shell->Mask; 
	unsigned int TypeFilter = MASK_RESERVED | MASK_PROJECTILE | MASK_WARHEAD | MASK_SHIP | MASK_STATIC | MASK_INANIMATE; 

	switch (shell->Mask & TypeFilter)
	{
		case MASK_PROJECTILE:
			// Bullet/Warhead
			switch (shell->SubTypeID)
			{
				
				case PROJECTILE_EMP:
				case PROJECTILE_SLUG:
				case PROJECTILE_FLECHETTE:
				case PROJECTILE_BULLET:
				{
					ProjectileRenderObject* ro = new ProjectileRenderObject();
					ro->Init(parentScene->getSceneManager(), obj, shell->Position, shell->Rotation, PROJECTILE_BULLET);
					obj->SetRenderObject(ro);
					obj->SetParentScene(parentScene);
					break;
				}
				default:
				break;
			}
			break;
		case MASK_WARHEAD:
			switch (shell->SubTypeID)
				{
					case WARHEAD_MISSILE:
					case WARHEAD_TORPEDO:
					case WARHEAD_EMP:
					case WARHEAD_ROCKET:
					{
						WarheadRenderObject* ro = new WarheadRenderObject();
						ro->Init(parentScene->getSceneManager(), obj, shell->Position, shell->Rotation, WARHEAD_ROCKET);
						obj->SetRenderObject(ro);
						obj->SetParentScene(parentScene);
						break;
					}
					default:
					break;
				}
			break;
		case MASK_SHIP:
		{
			ShipRenderObject* sro = new ShipRenderObject();
			sro->Init(parentScene->getSceneManager(), obj);
			sro->SetPosition(shell->Position);
			sro->SetRotation(shell->Rotation);

			obj->SetRenderObject(sro);
			obj->SetParentScene(parentScene);

			if (parentScene->GetAgent() && (parentScene->GetAgent()->GetID() == shell->AgentID))
			{
				PropertyState SensorRange(50000.0f, 0.0f, 50000.0f, PROP_SENSOR_RANGE, L"SensorRange");
				obj->AddPropertyState(PROP_SENSOR_RANGE, SensorRange); 
				PropertyState SensorSpeed(10.0f, 0.1f, 4.0f, PROP_SENSOR_SPEED, L"SensorSpeed");
				obj->AddPropertyState(PROP_SENSOR_SPEED, SensorSpeed); 
				PropertyState SensorAccuracy(1.0f, 0.1f, 0.2f, PROP_SENSOR_ACCURACY, L"SensorAccuracy");
				obj->AddPropertyState(PROP_SENSOR_ACCURACY, SensorAccuracy); 

				obj->SetTargetter(new TargettingModule(obj, parentScene));
				
				ClientHumanActuator* act = new ClientHumanActuator(obj->ID);
				obj->SetActuator(act);
				act->create(sro->GetSceneNode(), parentScene->getSceneManager());
				parentScene->GetAgent()->SetSpaceObject(obj);
				parentScene->GetAgent()->Init();
			}
				
			break;
		}
	default:
		break;
	}


	parentScene->AddSpaceObject(obj->ID, obj);
	return obj->ID;
}