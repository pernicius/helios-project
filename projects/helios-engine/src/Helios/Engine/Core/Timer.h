//==============================================================================
// High-Resolution Timer
//
// Provides a templated timer class built on std::chrono::high_resolution_clock
// with configurable time resolution (seconds, milliseconds, microseconds).
// Supports start/stop/reset operations and elapsed time queries. Returns
// elapsed time as floating-point values in the specified Period. Useful for
// performance measurements and frame timing.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Changelog:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include <chrono>

namespace Helios::Engine {


	template<typename Period = std::milli>
	class Timer
	{
	public:
		using resolution_t = std::chrono::duration<float, Period>;

		Timer()
			: m_StartTimePoint(std::chrono::high_resolution_clock::now())
			, m_Stopped(false)
		{
		}

		float Stop()
		{
			m_Stopped = true;
			m_EndTimePoint = std::chrono::high_resolution_clock::now();
			return Elapsed();
		}

		void Reset()
		{
			m_StartTimePoint = std::chrono::high_resolution_clock::now();
			m_Stopped = false;
		}

		float Elapsed() const
		{
			auto endTimePoint = m_Stopped ? m_EndTimePoint : std::chrono::high_resolution_clock::now();
			return std::chrono::duration<float, Period>(endTimePoint - m_StartTimePoint).count();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTimePoint;
		bool m_Stopped = false;
	};


	// some fixed aliases
	using TimerSec   = Timer<std::ratio<1>>;
	using TimerMilli = Timer<std::milli>;
	using TimerMicro = Timer<std::micro>;


} // namespace Helios::Engine
