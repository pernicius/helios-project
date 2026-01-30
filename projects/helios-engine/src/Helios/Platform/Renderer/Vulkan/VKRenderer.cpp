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
		m_framesCount = m_vkSwapchain->GetImageCount();

		CreateSimpleRenderPass();
		m_vkSwapchain->CreateFramebuffers(m_vkRenderPass->Get());

		CreateSimpleGraphicsPipeline();

		CreateCommandPool();
		CreateCommandBuffers();
		CreateSyncObjects();
	}


	void VKRenderer::Shutdown()
	{
		LOG_RENDER_INFO("Shutting down Vulkan Renderer...");

		m_vkDeviceManager->GetLogicalDevice().waitIdle();

		// Destruction must happen in the reverse order of creation to respect dependencies.
		// The Scope<T> smart pointers will handle calling the destructors automatically
		// when they are reset.
		for (size_t i = 0; i < m_framesCount; i++) {
			m_vkDeviceManager->GetLogicalDevice().destroySemaphore(m_renderFinishedSemaphores[i]);
			m_vkDeviceManager->GetLogicalDevice().destroySemaphore(m_imageAvailableSemaphores[i]);
			m_vkDeviceManager->GetLogicalDevice().destroyFence(m_inFlightFences[i]);
		}
		m_vkDeviceManager->GetLogicalDevice().destroyCommandPool(m_commandPool);

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


	bool VKRenderer::BeginFrame()
	{
		vk::Device logicalDevice = m_vkDeviceManager->GetLogicalDevice();

		// Wait for the frame to be finished
		vk::Result waitResult = logicalDevice.waitForFences(1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
		if (waitResult != vk::Result::eSuccess) {
			// This is a critical error. The cause could be a device loss, a driver crash, or a timeout
			// (which is unlikely with UINT64_MAX but indicates a GPU hang).
			// In any case, the rendering state is now undefined and we cannot safely continue.
			LOG_RENDER_EXCEPT("Failed to wait for fence! Result: {}", vk::to_string(waitResult));
		}

		// Acquire an image from the swap chain
		try {
			vk::ResultValue result = logicalDevice.acquireNextImageKHR(m_vkSwapchain->GetSwapchain(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], nullptr);
			m_currentImageIndex = result.value;
		}
		catch (const vk::OutOfDateKHRError&) {
			m_vkSwapchain->Recreate(*m_Window, m_vkRenderPass->Get());
			m_stateBeginFrameSuccess = false;
			return m_stateBeginFrameSuccess;
		}

		// Reset the fence only when we are about to submit new work
		vk::Result resetResult = logicalDevice.resetFences(1, &m_inFlightFences[m_currentFrame]);
		if (resetResult != vk::Result::eSuccess) {
			LOG_RENDER_EXCEPT("Failed to reset fence! Result: {}", vk::to_string(resetResult));
		}

		// Record the command buffer
		vk::CommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];
		commandBuffer.reset();

		commandBuffer.begin(vk::CommandBufferBeginInfo{});

		vk::ClearValue clearColor(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
		vk::RenderPassBeginInfo renderPassInfo = vk::RenderPassBeginInfo()
			.setRenderPass(m_vkRenderPass->Get())
			.setFramebuffer(m_vkSwapchain->GetFramebuffers()[m_currentImageIndex]->Get())
			.setRenderArea({ {0, 0}, m_vkSwapchain->GetExtent() })
			.setClearValueCount(1)
			.setPClearValues(&clearColor);

		commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_vkPipeline->GetPipeline());

		vk::Viewport viewport = vk::Viewport()
			.setX(0.0f)
			.setY(0.0f)
			.setWidth(static_cast<float>(m_vkSwapchain->GetExtent().width))
			.setHeight(static_cast<float>(m_vkSwapchain->GetExtent().height))
			.setMinDepth(0.0f)
			.setMaxDepth(1.0f);
		commandBuffer.setViewport(0, { viewport });

		vk::Rect2D scissor = vk::Rect2D()
			.setOffset({ 0, 0 })
			.setExtent(m_vkSwapchain->GetExtent());
		commandBuffer.setScissor(0, { scissor });

		m_stateBeginFrameSuccess = true;
		return m_stateBeginFrameSuccess;
	}


	void VKRenderer::EndFrame()
	{
		if (!m_stateBeginFrameSuccess) {
			return;
		}

		vk::CommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];
		commandBuffer.endRenderPass();
		commandBuffer.end();

		// Submit the command buffer
		vk::Semaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		vk::Semaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(waitSemaphores)
			.setPWaitDstStageMask(waitStages)
			.setCommandBufferCount(1)
			.setPCommandBuffers(&commandBuffer)
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(signalSemaphores);

		m_vkDeviceManager->GetGraphicsQueue().submit({ submitInfo }, m_inFlightFences[m_currentFrame]);

		// Present the image
		vk::SwapchainKHR swapChains[] = { m_vkSwapchain->GetSwapchain() };
		vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(signalSemaphores)
			.setSwapchainCount(1)
			.setPSwapchains(swapChains)
			.setPImageIndices(&m_currentImageIndex);

		vk::Result presentResult = vk::Result::eSuccess;
		try {
			presentResult = m_vkDeviceManager->GetPresentQueue().presentKHR(presentInfo);
		}
		catch (const vk::OutOfDateKHRError&) {
			presentResult = vk::Result::eErrorOutOfDateKHR;
		}

		if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR) {
			m_vkSwapchain->Recreate(*m_Window, m_vkRenderPass->Get());
		}

		m_currentFrame = (m_currentFrame + 1) % m_framesCount;
	}


	void VKRenderer::DrawFrame()
	{
		if (!m_stateBeginFrameSuccess) {
			return;
		}

		vk::CommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];

		commandBuffer.draw(3, 1, 0, 0);
	}


	bool VKRenderer::OnFramebufferResize(const FramebufferResizeEvent& e)
	{
		LOG_RENDER_DEBUG("VKRenderer: Framebuffer resize event received: {}, {}", e.width, e.height);
		if (m_vkSwapchain) {
			m_vkDeviceManager->GetLogicalDevice().waitIdle();
			m_vkSwapchain->Recreate(*m_Window, m_vkRenderPass->Get());
		}
		return false;
	}


	void VKRenderer::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = m_vkDeviceManager->GetQueueFamilyIndices();

		vk::CommandPoolCreateInfo poolInfo = vk::CommandPoolCreateInfo()
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			.setQueueFamilyIndex(queueFamilyIndices.graphics.value());

		m_commandPool = m_vkDeviceManager->GetLogicalDevice().createCommandPool(poolInfo);
		LOG_RENDER_DEBUG("VKRenderer: Command pool created.");
	}


	void VKRenderer::CreateCommandBuffers()
	{
		m_commandBuffers.resize(m_framesCount);

		vk::CommandBufferAllocateInfo allocInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool(m_commandPool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount((uint32_t)m_commandBuffers.size());

		m_commandBuffers = m_vkDeviceManager->GetLogicalDevice().allocateCommandBuffers(allocInfo);
		LOG_RENDER_DEBUG("VKRenderer: Command buffers allocated.");
	}


	void VKRenderer::CreateSyncObjects()
	{
		m_imageAvailableSemaphores.resize(m_framesCount);
		m_renderFinishedSemaphores.resize(m_framesCount);
		m_inFlightFences.resize(m_framesCount);

		vk::SemaphoreCreateInfo semaphoreInfo = {};
		vk::FenceCreateInfo fenceInfo = vk::FenceCreateInfo()
			.setFlags(vk::FenceCreateFlagBits::eSignaled); // Create fences in signaled state

		for (size_t i = 0; i < m_framesCount; i++) {
			m_imageAvailableSemaphores[i] = m_vkDeviceManager->GetLogicalDevice().createSemaphore(semaphoreInfo);
			m_renderFinishedSemaphores[i] = m_vkDeviceManager->GetLogicalDevice().createSemaphore(semaphoreInfo);
			m_inFlightFences[i] = m_vkDeviceManager->GetLogicalDevice().createFence(fenceInfo);
		}
		LOG_RENDER_DEBUG("VKRenderer: Synchronization objects created.");
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
