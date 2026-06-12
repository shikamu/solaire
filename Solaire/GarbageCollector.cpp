#include "GarbageCollector.h"
#include "PhysicsController.h"
#include "PhysicsObject.h"
#include "System.h"
#include "LogicScene.h"

#include "NetworkController.h"
#include "LANServer.h"
#include "Timer.h"

GarbageCollector GarbageCollector::m_Collector;

GarbageCollector& GarbageCollector::Get()
{
	return m_Collector;
}

GarbageCollector::GarbageCollector() : m_IsPaused (true)
{
}

GarbageCollector::~GarbageCollector()
{
	finish();
}

void GarbageCollector::SetPause(bool value)
{
	m_IsPaused = value; 
}

void GarbageCollector::acquireLock()
{
	m_lock.Lock();
}

void GarbageCollector::releaseLock()
{
	m_lock.Unlock();
}

unsigned int __stdcall GarbageCollector::run()
{
	Timer timer;
	timer.start();
	double currentTime = timer.getTimeSec();
	while (!isInterrupted())
	{
		Sleep(3000);
		/*
		double newTime = timer.getTimeSec();
		if (newTime - currentTime < 0.00033) continue;
		currentTime = newTime;
		*/
		if (m_IsPaused) continue;

		acquireLock();
		LogicScene* current = System::get().getCurrentScene();
		if(current)
			current->AcquireLock();
		PhysicsController::Get().AcquireLock();
		LANServer* server = NetworkController::get().getServer();
		if(server)
		{
			server->AcquireLock();

			server->CleanupShells();
		}
		PhysicsController::Get().CleanupObjects();
		current->CleanupSpaceObjects();

		if(server)
		{
			server->ReleaseLock();
		}
		PhysicsController::Get().ReleaseLock();
		if(current)
			current->ReleaseLock();
		//System::get().getCurrentScene()->ReleaseLock(); 
		releaseLock();
	}

	return 0; 
}
