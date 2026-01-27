//==============================================================================
// Vulkan Renderer Implementation
//
// Concrete implementation of the Renderer interface for the Vulkan API. It is
// responsible for creating and managing the lifecycle of all core Vulkan
// objects, including the instance, surface, device manager, swapchain, and
// command buffers, orchestrating them to perform rendering operations.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - Implements the abstract `Renderer` interface for Vulkan.
// - Manages the entire lifecycle of core Vulkan components.
// - Initializes the Vulkan API in the correct order (Instance -> Surface -> Device).
// - Handles graceful shutdown and resource destruction.
// - Serves as the central hub for all Vulkan-specific rendering logic.
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include "Helios/Engine/Renderer/Renderer.h"

#include "Helios/Platform/Renderer/Vulkan/VKInstance.h"
// #include "Helios/Platform/Renderer/Vulkan/VKSurface.h"
// #include "Helios/Platform/Renderer/Vulkan/VKDeviceManager.h"
// ... other Vulkan headers

namespace Helios::Engine::Renderer::Vulkan {


	class VKRenderer : public Renderer
	{
	public:
		VKRenderer() = default;
		virtual ~VKRenderer() = default;

		virtual void Init(const AppSpec& appSpec, Window& window) override;
		virtual void Shutdown() override;

	private:
		Scope<VKInstance> m_vkInstance;
		// Scope<VKSurface> m_vkSurface;
		// Scope<VKDeviceManager> m_vkDeviceManager;
		// ... other Vulkan objects
	};


} // namespace Helios::Engine::Renderer::Vulkan
