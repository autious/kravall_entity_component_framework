#pragma once

#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#define WCLOCK
#endif

namespace Core
{
    class HighresTimer
    {
#ifndef WCLOCK
        std::chrono::high_resolution_clock::time_point m_start, m_end;
        std::chrono::high_resolution_clock::time_point m_totalStart;
#else
		private:

		LARGE_INTEGER m_frequency;        // ticks per second
		LARGE_INTEGER m_totalStart;
		LARGE_INTEGER m_start, m_end;           // ticks
#endif
        long long m_totalTime;
    public:
        HighresTimer();

        /*!
        Starts measuring time
        */
        void Start();

        /*!
        Stops measuring time
        */
        void Stop();

        /*!
        Resets the total time to now
        */
        void Reset();

        /*!
        Gets the delta time
        \return Returns time passed between calling Start and Stop
        */
        std::chrono::microseconds GetDelta();

        /*!
        Gets the total time
        \return Returns the total time passed since the application started
        */
        long long GetTotal();

    };
}