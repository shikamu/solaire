#pragma once
#include "CSLock.h"


template <class T> 
class LockPointer
{
private:
	T *m_Object;
	CSLock m_Lock;
	LockPointer& operator= (const LockPointer& other)
	{
	} 
	LockPointer(const LockPointer& other)
	{
	}
public:
	LockPointer(T* inObject) : m_Object (inObject) {}

	~LockPointer() 
	{
		if (m_Object)
		{
			m_Lock.Lock();
			delete m_Object;
			m_Object = NULL;
			m_Lock.Unlock();
		}
	}


	T* GetPointer() 
	{
		m_Lock.Lock();
		return m_Object;
	}
	void Lock()
	{
		m_Lock.Lock();
	}
	void Unlock()
	{
		m_Lock.Unlock();
	}
	T* GetUnlockedPointer()
	{
		return m_Object;
	}
};

