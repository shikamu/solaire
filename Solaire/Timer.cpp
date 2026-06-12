#include "Timer.h"

Timer::Timer() : m_stime(0), m_etime(0), m_stopped(true)
{
    QueryPerformanceFrequency(&m_freq);
    m_scount.QuadPart = 0;
    m_ecount.QuadPart = 0;
    m_stopped = true;
}

Timer::~Timer()
{

}

void Timer::start()
{
    m_stopped = false;
    QueryPerformanceCounter(&m_scount);
}

void Timer::resume()
{
	m_stopped = false;
}

void Timer::stop()
{
    m_stopped = true;
    QueryPerformanceCounter(&m_ecount);
}

double Timer::getTimeSec()
{
    if(!m_stopped)
        QueryPerformanceCounter(&m_ecount);

    return static_cast<double>(m_ecount.QuadPart - m_scount.QuadPart) / m_freq.QuadPart;
}

double Timer::getTimeMilliSec()
{
	return getTimeSec() * 1000.0;
}

double Timer::getTimeMicroSec()
{
	return getTimeSec() * 1000000.0;
}
