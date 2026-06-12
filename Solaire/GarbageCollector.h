#pragma once
#include "Task.h"
#include "CSLock.h"

class GarbageCollector : public Task
{
public:
	static GarbageCollector& Get(); 
	void SetPause(bool value); 

	//TODO see if these functions are really necessary
	void acquireLock();
	void releaseLock();


protected:
	unsigned int __stdcall run();
private:
	GarbageCollector();
	~GarbageCollector(); 

	GarbageCollector(const GarbageCollector& other);
	GarbageCollector& operator=(const GarbageCollector& other);


	static GarbageCollector m_Collector;

	CSLock m_lock;

	bool m_IsPaused; 

};

