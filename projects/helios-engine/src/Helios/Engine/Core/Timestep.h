//==============================================================================
// Timestep Wrapper
//
// Provides a lightweight wrapper for delta time values passed to layer update
// callbacks. Stores time in seconds as a float and provides implicit conversion
// to float for convenient arithmetic operations. Designed for use with frame
// timing and fixed timestep game loops.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

namespace Helios::Engine {


	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: m_Time(time)
		{
		}

		operator float() const { return m_Time; }
//		float GetSeconds() const { return m_Time; }
//		float GetMilliseconds() const { return m_Time * 1000.0f; }

	private:
		float m_Time;
	};


} // namespace Helios::Engine
