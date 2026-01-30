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
#include "Helios/Engine/VFS/VFS.h"

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

// Tempoary!!!
		VirtFS.Mount("assets", appSpec.WorkingDirectory + "/../helios-engine/assets", 0, "HeliosEngine");
		VirtFS.CreateAlias("@assets:", "assets");
// Tempoary!!!

		// The initialization order is critical and follows the Vulkan object dependency chain.

		DEBUG_FILTER_ID(0); // Massive clutter
		DEBUG_FILTER_ID(601872502); // Khronos Validation Layer Active...
		m_vkInstance = VKInstanceBuilder()
			.SetAppSpec(appSpec)
			.WithGlfwExtensions()
			.Build();
		DEBUG_FILTER_RESET();

		m_vkSurface = CreateScope<VKSurface>(*m_vkInstance, *m_Window);

		DEBUG_FILTER_ID(0); // Massive clutter
		m_vkDeviceManager = CreateScope<VKDeviceManager>(*m_vkInstance, *m_vkSurface);
		DEBUG_FILTER_RESET();

		m_vkSwapchain = CreateScope<VKSwapchain>(*m_vkDeviceManager, *m_vkSurface, *m_Window);

		CreateSimpleRenderPass();
		m_vkSwapchain->CreateFramebuffers(m_vkRenderPass->Get());

		CreateSimpleGraphicsPipeline();
	}


	void VKRenderer::Shutdown()
	{
		LOG_RENDER_INFO("Shutting down Vulkan Renderer...");

		// Destruction must happen in the reverse order of creation to respect dependencies.
		// The Scope<T> smart pointers will handle calling the destructors automatically
		// when they are reset.

		m_vkPipeline.reset();
		m_vkRenderPass.reset();
		m_vkSwapchain.reset();
		m_vkDeviceManager.reset();
		m_vkSurface.reset();

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
			m_vkSwapchain->Recreate(*m_Window, m_vkRenderPass->Get());
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


	void VKRenderer::CreateSimpleGraphicsPipeline()
	{
		// Viewport and scissor will be dynamic
		vk::Viewport viewport = {};
		vk::Rect2D scissor = {};

		// Color blend attachment state
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
			.setBlendEnable(VK_FALSE);

		VKPipelineBuilder builder(*m_vkDeviceManager, *m_vkRenderPass);
		builder.SetShaders("@assets:/shaders/vulkan/simple.vert.spv", "@assets:/shaders/vulkan/simple.frag.spv")
			.SetVertexInput({}, {}) // No vertex input for now
			.SetInputAssembly(vk::PrimitiveTopology::eTriangleList)
			.SetViewport(viewport)
			.SetScissor(scissor)
			.SetRasterizer(vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise)
			.SetMultisampling()
			.SetColorBlending(colorBlendAttachment)
			.SetDepthStencil(VK_FALSE, VK_FALSE, vk::CompareOp::eLess)
			.SetDynamicState({ vk::DynamicState::eViewport, vk::DynamicState::eScissor });
		m_vkPipeline = builder.Build();
	}


} // namespace Helios::Engine::Renderer::Vulkan
