#include <iostream>
#include "PhysicsController.h"
#include "Timer.h"
#include "PhysicsListener.h"
#include "LockPointer.h"
#include "LogicConstants.h"


PhysicsController PhysicsController::m_PhysicsController;

PhysicsController& PhysicsController::Get()
{
	return m_PhysicsController;
}
PhysicsController::~PhysicsController() 
{
	finish();
} 

void PhysicsController::ForcedObjectsCleanup()
{
	m_ListLock.Lock();
	for (auto i = m_ObjectList.begin(); i < m_ObjectList.end(); i++)
	{
		//delete (*i)->GetPointer();
		delete (*i);
	}
	m_ObjectList.clear();
	m_ListLock.Unlock();

}

void PhysicsController::finish()
{
	Task::finish();
	//ForcedObjectsCleanup();
}

unsigned int __stdcall PhysicsController::run()
{	
	Timer physicsTimer;
	physicsTimer.start();
	float currentTime = (float)physicsTimer.getTimeSec();
	while (!isInterrupted())
	{
		float newTime = (float)physicsTimer.getTimeSec();
		if (newTime - currentTime < 0.001f) continue; 
		Update(newTime - currentTime);
		currentTime = newTime;
		//Sleep(5);
	}

	return 1; 
}

LockPointer<PhysicsObject>* PhysicsController::CreateObject(PhysicsParams& params)
{
	PhysicsObject* obj = new PhysicsObject();
	obj->Init(params);
	LockPointer<PhysicsObject> *p = new LockPointer<PhysicsObject>(obj);
	m_ListLock.Lock();
	m_ObjectList.push_back(p);
	m_ListLock.Unlock();
	return p;
}


void PhysicsController::CleanupObjects()
{
	m_ListLock.Lock();
	for (auto i = m_ObjectList.begin(); i < m_ObjectList.end();)
	{
		if ((*i)->GetPointer()->CheckCleanupState())
		{
			//if ((*i)->GetPointer()->m_Parameters.Listener) 
			//{
			//	(*i)->GetPointer()->m_Parameters.Listener = NULL; 
			//}
			//delete (*i)->GetPointer();
			delete (*i);
			i = m_ObjectList.erase(i);
		}
		else
		{
			(*i)->Unlock();
			i++;
		}
	}
	m_ListLock.Unlock();
}

void PhysicsController::Update(const float deltaTime)
{
	m_ListLock.Lock();
	for (auto i = m_ObjectList.begin(); i < m_ObjectList.end(); i++)
	{
		PhysicsObject *obj = (*i)->GetPointer();
		if (!obj->CheckCleanupState()) obj->Update(deltaTime);
		(*i)->Unlock(); 
	}
	
	for (auto i = m_ObjectList.begin(); i < m_ObjectList.end(); i++)
	{	
		PhysicsObject* ObjectA = (*i)->GetUnlockedPointer();
		if (ObjectA->CheckCleanupState()) continue;
		//PhysicsObject* obj1 = (*i)->GetPointer();
		for (auto j = i + 1; j < m_ObjectList.end(); j++)
		{ 
			PhysicsObject* ObjectB = (*j)->GetUnlockedPointer();
			if (ObjectA->CheckCleanupState() || ObjectB->CheckCleanupState()) continue;
			// Compare filters and ignore if necessary
			MaskComparison Comparison = MaskCheck(*ObjectA,*ObjectB);
			if (Comparison == MASK_IGNORE) continue;
			

			// Static collision detection: Distance between objects compared to combined sizes (radius)
			if ((ObjectA->GetPosition() - ObjectB->GetPosition()).getLength() >
				ObjectA->m_Parameters.Size + ObjectB->m_Parameters.Size) continue; 
			
			PhysicsObject* LockedA = (*i)->GetPointer();
			PhysicsObject* LockedB = (*j)->GetPointer();

			vector3df CollisionNormal(LockedA->GetPosition() - LockedB->GetPosition());
			CollisionNormal.normalize();
			float PenetrationDistance = LockedA->m_Parameters.Size + LockedB->m_Parameters.Size -
				(LockedA->GetPosition() - LockedB->GetPosition()).getLength();
			
			float TotalMass = LockedA->m_Parameters.Mass + LockedB->m_Parameters.Mass;

			vector3df CorrectionVectorA(LockedA->GetPosition() + CollisionNormal * PenetrationDistance * LockedB->m_Parameters.Mass / TotalMass);
			vector3df CorrectionVectorB(LockedB->GetPosition() - CollisionNormal * PenetrationDistance * LockedA->m_Parameters.Mass / TotalMass);

			LockedA->SetPosition(CorrectionVectorA);
			LockedB->SetPosition(CorrectionVectorB);

			(*i)->Unlock();
			(*j)->Unlock();
			// Collision Response Code
			// DO NOT USE ObjectA/B, use locked pointers!
			///////////////////////////
			/*
			PhysicsObject* obj2 = (*j)->GetPointer();

			
			//inertia
			float IA = obj1->m_Parameters.Size*obj1->m_Parameters.Size*obj1->m_Parameters.Mass;
			float IB = obj2->m_Parameters.Size*obj2->m_Parameters.Size*obj2->m_Parameters.Mass;

			//relative velocities
			vector3df vrel = obj2->GetVelocity()-obj1->GetVelocity();
			
			//coefficient of restitution
			float epsilon = 0.5f;
			
			vector3df N = (obj2->GetPosition() - obj1->GetPosition()).normalize(); //collision normal
			vector3df P = N*obj1->m_Parameters.Size;	//point of contact
			//N = obj2->GetPosition().crossProduct(obj1->GetPosition());
			N = obj2->GetPosition().crossProduct(P);

			float numerator = -(1+epsilon) * vrel.dotProduct(N);
			
			vector3df ra = P-obj1->GetPosition();
			vector3df rb = P-obj2->GetPosition();
			float term1 = obj1->m_Parameters.MassInverse + obj2->m_Parameters.MassInverse;
			float term2 = N.dotProduct((1.0f*IA*(ra.crossProduct(N))).crossProduct(ra));
			float term3 = N.dotProduct((1.0f*IB*(rb.crossProduct(N))).crossProduct(rb));

			float denominator = term1+term2+term3;
			float impulseFactor = numerator/denominator;
			vector3df impulse = N*impulseFactor;
			obj1->ApplyImpulse(impulse);
			obj2->ApplyImpulse(-impulse);
			(*j)->Unlock();
			*/
			///////////////////////////

			// Call game logic functions
			
			if (Comparison == MASK_COLLISION_LOGIC)
			{
				ObjectA->m_Parameters.Listener->OnCollide(ObjectB->m_Parameters.ID, ObjectB->m_Parameters.ParentID);
				ObjectB->m_Parameters.Listener->OnCollide(ObjectA->m_Parameters.ID, ObjectA->m_Parameters.ParentID);
			}
			
			if (ObjectA->m_Parameters.IsVolatile)
			{
				ObjectA->MarkForCleanup();
				ObjectA->m_Parameters.Listener->OnDestroy();
				// Set Listener to Null
			}
			if (ObjectB->m_Parameters.IsVolatile) 
			{
 				ObjectB->MarkForCleanup();
				ObjectB->m_Parameters.Listener->OnDestroy();
			}
			
		}
		//(*i)->Unlock();
	}
	

	m_ListLock.Unlock();

	//CleanupObjects();
}

MaskComparison PhysicsController::MaskCheck(const PhysicsObject &A, const PhysicsObject &B)
{
	unsigned int MaskA = A.m_Parameters.ObjectMask;
	unsigned int MaskB = B.m_Parameters.ObjectMask;
	if (A.m_Parameters.ID == B.m_Parameters.ParentID || A.m_Parameters.ParentID == B.m_Parameters.ID || (((MaskA & MaskB) & MASK_PROJECTILE) != 0)) return MASK_IGNORE;
	
	if ((((MASK_SHIP | MASK_STATIC) & (MaskA | MaskB)) == 0 ) || ((MaskA & GROUP_FILTER) == (MaskB & GROUP_FILTER))) return MASK_COLLISION;
	
	return MASK_COLLISION_LOGIC;
	//std::cout << "N Physics:	" << m_ObjectList.size() << std::endl;
}

void PhysicsController::AcquireLock()
{
	m_ListLock.Lock();
}
void PhysicsController::ReleaseLock()
{
	m_ListLock.Unlock();
}