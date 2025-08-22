#pragma once

namespace Helios::Engine {


	class Timer
	{
	public:
		Timer() { Reset(); }

		void Reset() { m_Start = std::chrono::high_resolution_clock::now(); }

		// Get elapsed seconds
		float Elapsed() { return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f; }

		// Get elepsed milliseconds
		float ElapsedMS() { return Elapsed() * 1000.0f; }

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};


} // namespace Helios::Engine
