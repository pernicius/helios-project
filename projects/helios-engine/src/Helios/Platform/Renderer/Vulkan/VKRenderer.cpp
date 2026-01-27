//==============================================================================
// Vulkan Renderer Implementation (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VKRenderer.h
//==============================================================================
#include "pch.h"
#include "Helios/Platform/Renderer/Vulkan/VKRenderer.h"

namespace Helios::Engine::Renderer::Vulkan {


	void VKRenderer::Init(const AppSpec& appSpec, Window& window)
	{
		LOG_RENDER_INFO("Initializing Vulkan Renderer...");

		// The initialization order is critical and follows the Vulkan object dependency chain.
		// 1. Create the Vulkan Instance
		m_vkInstance = CreateScope<VKInstance>(appSpec);

		// 2. Create the Window Surface (next step)
		// m_vkSurface = CreateScope<VKSurface>(m_vkInstance, window);

		// 3. Create the Device Manager (selects physical device, creates logical device)
		// m_vkDeviceManager = CreateScope<VKDeviceManager>(m_vkInstance, m_vkSurface);

		// 4. Create the Swapchain
		// ...and so on for the rest of the rendering pipeline.
	}


	void VKRenderer::Shutdown()
	{
		LOG_RENDER_INFO("Shutting down Vulkan Renderer...");

		// Destruction must happen in the reverse order of creation to respect dependencies.
		// The Scope<T> smart pointers will handle calling the destructors automatically
		// when they are reset.

		// 7. Destroy Framebuffers, Command Buffers, etc.

		// 6. Destroy Pipeline and RenderPass

		// 5. Destroy Swapchain

		// 4. Destroy Device Manager
		// m_vkDeviceManager.reset();

		// 3. Destroy Surface
		// m_vkSurface.reset();

		// 2. Destroy Instance (which also handles the debug messenger)
		m_vkInstance.reset();
	}


} // namespace Helios::Engine::Renderer::Vulkan
