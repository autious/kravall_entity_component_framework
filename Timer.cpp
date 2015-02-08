#include "Timer.hpp"

namespace Core
{
    HighresTimer::HighresTimer()
    {
#ifndef WCLOCK
        m_totalStart = std::chrono::high_resolution_clock::now();
#else
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_totalStart);
	#endif
    }

    void HighresTimer::Start()
    {
#ifndef WCLOCK
        m_start = std::chrono::high_resolution_clock::now();
#else
		QueryPerformanceCounter(&m_start);
	#endif
    }

    void HighresTimer::Stop()
    {
#ifndef WCLOCK
        m_end = std::chrono::high_resolution_clock::now();
#else
		QueryPerformanceCounter(&m_end);
	#endif
    }


    void HighresTimer::Reset()
    {
#ifndef WCLOCK
        m_totalStart = std::chrono::high_resolution_clock::now();
#else
		QueryPerformanceCounter(&m_totalStart);
	#endif
    }

    std::chrono::microseconds HighresTimer::GetDelta()
    {
#ifndef WCLOCK
        return std::chrono::duration_cast<std::chrono::microseconds>( m_end - m_start );
#else
		double elapsed = ((m_end.QuadPart - m_start.QuadPart)*1E6)/static_cast<double>(m_frequency.QuadPart);
		std::chrono::duration<double, std::micro> ms(elapsed);
		return std::chrono::duration_cast<std::chrono::microseconds>( ms );
	#endif
    }

    long long HighresTimer::GetTotal()
    {
#ifndef WCLOCK
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>( now - m_totalStart ).count();
#else
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		long long elapsed = 1000 * (now.QuadPart - m_totalStart.QuadPart)/m_frequency.QuadPart;
		return elapsed;
	#endif
    }
}