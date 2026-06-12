#include <process.h>

#include "Task.h"

Task::Task() : m_threadHandle(NULL), m_threadId(0), m_threadStarted(false), m_interrupted(true)
{
	
}

void Task::start(const int priority, const unsigned int stackSize)
{
	if(!m_threadStarted)
	{
		m_threadHandle = (HANDLE)_beginthreadex(
			NULL,				// no security attributes (child cannot inherited handle)
			stackSize,			// stack size
			Task::threadFunc,	// code to run on new thread
			this,				// pointer to host application class
			0,					// run immediately (could create suspended)
			&m_threadId			// OUT: returns thread ID
		);
		m_threadStarted = m_threadHandle != NULL;
		if(m_threadHandle)
		{
			SetThreadPriority(m_threadHandle, priority);
			m_interrupted = false;
		}
	}
}

bool Task::isRunning()
{
	if(m_threadStarted && !m_interrupted)
	{
		DWORD code = WaitForSingleObject(m_threadHandle, 0);
		return code != WAIT_OBJECT_0;
	}
	return false;
}

void Task::finish()
{
	if(m_threadStarted)
	{
		interrupt();
		//WaitForSingleObject(m_threadHandle, 1000L);
		WaitForSingleObject(m_threadHandle, INFINITE);
		BOOL b = CloseHandle(m_threadHandle);
		m_threadHandle = NULL;
		m_threadId = 0;
		m_threadStarted = false;
	}
}

Task::~Task()
{
	finish();
}
