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

#include "Helios/Engine/Core/Config.h"

namespace Helios::Engine::Renderer::Vulkan {


	void VKRenderer::Init(const AppSpec& appSpec, Window& window)
	{
		LOG_RENDER_INFO("Initializing Vulkan Renderer...");

		m_Window = &window;

		// Load the configuration for the Vulkan renderer
		ConfigManager::GetInstance().LoadDomain("renderer_vulkan");

		// The initialization order is critical and follows the Vulkan object dependency chain.
		// 1. Create the Vulkan Instance
		m_vkInstance = CreateScope<VKInstance>(appSpec);

		// 2. Create the Window Surface
		m_vkSurface = CreateScope<VKSurface>(*m_vkInstance, *m_Window);

		// 3. Create the Device Manager (selects physical device, creates logical device)
		m_vkDeviceManager = CreateScope<VKDeviceManager>(*m_vkInstance, *m_vkSurface);

		// 4. Create the Swapchain
		m_vkSwapchain = CreateScope<VKSwapchain>(*m_vkDeviceManager, *m_vkSurface, *m_Window);
	}


	void VKRenderer::Shutdown()
	{
		LOG_RENDER_INFO("Shutting down Vulkan Renderer...");

		// Destruction must happen in the reverse order of creation to respect dependencies.
		// The Scope<T> smart pointers will handle calling the destructors automatically
		// when they are reset.

		// 6. Destroy Framebuffers, Command Buffers, etc.

		// 5. Destroy Pipeline and RenderPass

		// 4. Destroy Swapchain
		m_vkSwapchain.reset();

		// 3. Destroy Device Manager
		m_vkDeviceManager.reset();

		// 2. Destroy Surface
		m_vkSurface.reset();

		// 1. Destroy Instance (which also handles the debug messenger)
		m_vkInstance.reset();

		// Save the configuration for the Vulkan renderer
		ConfigManager::GetInstance().SaveDomain("renderer_vulkan");
	}


	void VKRenderer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<FramebufferResizeEvent>(HE_BIND_EVENT_FN(VKRenderer::OnFramebufferResize));
	}


	bool VKRenderer::OnFramebufferResize(const FramebufferResizeEvent& e)
	{
		LOG_RENDER_DEBUG("VKRenderer: Framebuffer resize event received: {}, {}", e.width, e.height);
		if (m_vkSwapchain) {
			m_vkSwapchain->RecreateSwapchain(*m_Window);
		}
		return false;
	}


} // namespace Helios::Engine::Renderer::Vulkan
