#pragma once 
#define WIN64_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
//#include <process.h>

class CSLock
{
private:
	CRITICAL_SECTION m_cs;
	//HANDLE m_mutex;

public:
	CSLock()
	{ 
		InitializeCriticalSection(&m_cs);
		//m_mutex = CreateMutex(NULL, false, NULL);
	}
	~CSLock() 
	{ 
		DeleteCriticalSection(&m_cs); 
		//CloseHandle(m_mutex); m_mutex = NULL;
		//
	} 
	void Lock() 
	{
		EnterCriticalSection(&m_cs); 
		//WaitForSingleObject(m_mutex, INFINITE);
	}
	void Unlock() 
	{ 
		LeaveCriticalSection(&m_cs); 
		//ReleaseMutex(m_mutex);
	} 
};