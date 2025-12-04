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


} // namespace Helios::Util
