#pragma once

#define WIN64_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>


class Timer
{
public:

    Timer();
    ~Timer();

    void start();
	void resume();
    void stop();

    double getTimeSec();
    double getTimeMilliSec();
    double getTimeMicroSec();

private:
    double m_stime; // starting time in micro-seconds
    double m_etime; // ending time in micro-seconds

    LARGE_INTEGER m_freq; // ticks per second
    LARGE_INTEGER m_scount; // start count
    LARGE_INTEGER m_ecount; // end count

	bool m_stopped; // stop flag
};
