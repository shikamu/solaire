#pragma once
#define WIN64_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

class Task
{
public:

	Task();
	virtual ~Task();

	static unsigned __stdcall threadFunc(void *param)
	{
		if (param)
			return ((Task*)param)->run();
		return 1;  // Return error
	}

	void start(const int priority = THREAD_PRIORITY_NORMAL, const unsigned int stackSize=1048576);//default 1MB stack size
	
	virtual void finish();

	bool isRunning();

	HANDLE getThreadHandle() const
	{
		return m_threadHandle;
	}

	void interrupt(){
		m_interrupted = true;
	}

	bool isInterrupted() const
	{
		return m_interrupted;
	}

protected:
	virtual unsigned int __stdcall run() = 0;

private:

	HANDLE m_threadHandle;
	unsigned int m_threadId;

	bool m_threadStarted, m_interrupted;
};

