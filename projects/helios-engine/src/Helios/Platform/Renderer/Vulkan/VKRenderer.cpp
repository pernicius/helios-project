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

#ifdef BUILD_DEBUG
#	define DEBUG_FILTER_ID(id) VKInstance::DisableDebugMessageID(id)
#	define DEBUG_FILTER_RESET() VKInstance::ResetDisabledDebugMessageIDs()
#else
#	define DEBUG_FILTER_ID(id)
#	define DEBUG_FILTER_RESET()
#endif

namespace Helios::Engine::Renderer::Vulkan {


	void VKRenderer::Init(const AppSpec& appSpec, Window& window)
	{
		LOG_RENDER_INFO("Initializing Vulkan Renderer...");

		m_Window = &window;

		// Load the configuration for the Vulkan renderer
		ConfigManager::GetInstance().LoadDomain("renderer_vulkan");

		// The initialization order is critical and follows the Vulkan object dependency chain.

		// 1. Create the Vulkan Instance
		DEBUG_FILTER_ID(0); // Massive clutter
		DEBUG_FILTER_ID(601872502); // Khronos Validation Layer Active...
		m_vkInstance = VKInstanceBuilder()
			.SetAppSpec(appSpec)
			.WithGlfwExtensions()
			.Build();
		DEBUG_FILTER_RESET();

		// 2. Create the Window Surface
		m_vkSurface = CreateScope<VKSurface>(*m_vkInstance, *m_Window);

		// 3. Create the Device Manager (selects physical device, creates logical device)
		DEBUG_FILTER_ID(0); // Massive clutter
		m_vkDeviceManager = CreateScope<VKDeviceManager>(*m_vkInstance, *m_vkSurface);
		DEBUG_FILTER_RESET();

		// 4. Create the Swapchain
		m_vkSwapchain = CreateScope<VKSwapchain>(*m_vkDeviceManager, *m_vkSurface, *m_Window);

		// 5. Create the Render Pass
		CreateSimpleRenderPass();
	}


	void VKRenderer::Shutdown()
	{
		LOG_RENDER_INFO("Shutting down Vulkan Renderer...");

		// Destruction must happen in the reverse order of creation to respect dependencies.
		// The Scope<T> smart pointers will handle calling the destructors automatically
		// when they are reset.

		// 7. Destroy Framebuffers, Command Buffers, etc.

		// 6. Destroy Pipeline

		// 5. Destroy RenderPass
		m_vkRenderPass.reset();

		// 4. Destroy Swapchain
		m_vkSwapchain.reset();

		// 3. Destroy Device Manager
		m_vkDeviceManager.reset();

		// 2. Destroy Surface
		m_vkSurface.reset();

		// 1. Destroy Instance (which also handles the debug messenger)
		DEBUG_FILTER_ID(0); // Massive clutter
		m_vkInstance.reset();
		DEBUG_FILTER_RESET();

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


	void VKRenderer::CreateSimpleRenderPass()
	{
		VKRenderPassBuilder builder(*m_vkDeviceManager);

		// Color attachment for the swapchain image
		vk::AttachmentDescription colorAttachment = vk::AttachmentDescription()
			.setFormat(m_vkSwapchain->GetImageFormat())
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

		builder.AddAttachment(colorAttachment);

		// Reference to the color attachment
		vk::AttachmentReference colorAttachmentRef = vk::AttachmentReference()
			.setAttachment(0) // Index of the attachment in the attachments array
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		// A single subpass using the color attachment
		builder.AddSubpass({ colorAttachmentRef });

		// Dependency to transition the image layout for presentation
		vk::SubpassDependency dependency = vk::SubpassDependency()
			.setSrcSubpass(VK_SUBPASS_EXTERNAL)
			.setDstSubpass(0)
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setSrcAccessMask({})
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

		builder.AddDependency(dependency);

		m_vkRenderPass = builder.Build();
	}

} // namespace Helios::Engine::Renderer::Vulkan
