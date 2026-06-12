#pragma once
#include <iostream>
#include <ctime>
#include <windows.h>
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")

class MMTimer
{
private:
	DWORD m_InitialTime, m_StartTime, m_CurrentTime, m_DeltaTime, m_ElapsedTime;

public:
	MMTimer() 
	{
		m_InitialTime = timeGetTime();
		timeBeginPeriod(1);

		m_ElapsedTime = 0x00000000;
	}
	~MMTimer() 
	{
		timeEndPeriod(1);
	}

	void ResetTimer()
	{
		m_InitialTime = timeGetTime(); 
		m_CurrentTime = m_InitialTime; 
	}
	
	double ElapsedTime()
	{
		return (double)(m_CurrentTime - m_InitialTime) * 0.001; 
	}

	void Update()
	{
		m_DeltaTime = timeGetTime() - m_CurrentTime;
		m_CurrentTime = timeGetTime(); 
	}
	
	double  CurrentTime()
	{
		return (double) m_CurrentTime * 0.001;
	}
	double InitialTime()
	{
		return (double) m_InitialTime * 0.001;
	}
	double DeltaTime()
	{
		return (double) m_DeltaTime * 0.001;
	}
};