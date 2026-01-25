//==============================================================================
// Layer System
//
// Provides an abstract base class for application layers in the layer stack.
// Layers receive lifecycle callbacks (OnAttach/OnDetach), per-frame updates
// (OnUpdate with timestep), rendering callbacks (OnRender), and event
// notifications (OnEvent). Enables modular composition of application
// functionality with well-defined execution order.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

namespace Helios::Engine {


	class Layer
	{
	public:
		Layer(const std::string& name = "Layer")
			: m_Name(name)
		{
		}
		virtual ~Layer() = default;
		[[nodiscard]] const std::string& GetName() const { return m_Name; }

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		
		virtual void OnUpdate(float /*timestep*/) {}
		virtual void OnRender() {}

		virtual void OnEvent(class Event& /*event*/) {}
		
	protected:
		std::string m_Name;
	};


} // namespace Helios::Engine
