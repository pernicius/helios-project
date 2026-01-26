//==============================================================================
// Layer Stack
//
// Manages an ordered collection of Layer objects with a two-tier system:
// regular layers (bottom half) and overlays (top half, always rendered last).
// Layers are inserted at m_LayerInsertIndex, overlays are always pushed to
// the end. Provides bidirectional iteration for forward (rendering) and
// reverse (event propagation) traversal. Owns the lifetime of all layers.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - Two-tier system: regular layers (bottom) + overlays (top)
// - Bidirectional iteration (forward for rendering, reverse for events)
// - Dynamic layer insertion/removal with lifetime management
// - Overlays always processed last for UI/debug priority
// - STL-compatible iterator interface
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include "Helios/Engine/Core/Layer.h"

namespace Helios::Engine {


	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
		std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
		std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }

		std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }
		std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }
	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};


} // namespace Helios::Engine
