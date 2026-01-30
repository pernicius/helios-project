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
// - Manages the lifecycle of core Vulkan objects (Instance, Surface, Device, etc.).
// - Coordinates the initialization and shutdown of the Vulkan API.
// - Handles events, such as window resizing, to recreate the swapchain.
// - Serves as the central hub for Vulkan-specific rendering logic.
// 
// Changelog:
// - 2026.01: Refactored rendering loop into BeginFrame/DrawFrame/EndFrame
// - 2026.01: Added basic rendering loop with command buffers and synchronization
// - 2026.01: Added framebuffer creation in swapchain
// - 2026.01: Added graphics pipeline management
// - 2026.01: Refactored to use the builder pattern for RenderPass creation
// - 2026.01: Added RenderPass management
// - 2025.01: Added Configuration support via ConfigManager
// - 2026.01: Added Event handling for window resize
// - 2026.01: Added Swapchain management
// - 2026.01: Added Device management
// - 2026.01: Added Surface management
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include "Helios/Engine/Renderer/Renderer.h"

#include "Helios/Platform/Renderer/Vulkan/VKInstance.h"
#include "Helios/Platform/Renderer/Vulkan/VKSurface.h"
#include "Helios/Platform/Renderer/Vulkan/VKDeviceManager.h"
#include "Helios/Platform/Renderer/Vulkan/VKSwapchain.h"
#include "Helios/Platform/Renderer/Vulkan/VKRenderPass.h"
#include "Helios/Platform/Renderer/Vulkan/VKPipeline.h"
#include "Helios/Platform/Renderer/Vulkan/VKFramebuffer.h"

namespace Helios::Engine::Renderer::Vulkan {


	class VKRenderer : public Renderer
	{
	public:
		VKRenderer() = default;
		virtual ~VKRenderer() = default;

		virtual void Init(const AppSpec& appSpec, Window& window) override;
		virtual void Shutdown() override;

		virtual void OnEvent(Event& e) override;

		virtual bool BeginFrame() override;
		virtual void EndFrame() override;
		virtual void DrawFrame() override;

	private:
		bool OnFramebufferResize(const FramebufferResizeEvent& e);

		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		// Tempoary
		void CreateSimpleRenderPass();
		void CreateSimpleGraphicsPipeline();

	private:
		Window* m_Window = nullptr;
		Scope<VKInstance> m_vkInstance;
		Scope<VKSurface> m_vkSurface;
		Scope<VKDeviceManager> m_vkDeviceManager;
		Scope<VKSwapchain> m_vkSwapchain;
		Scope<VKRenderPass> m_vkRenderPass;
		Scope<VKPipeline> m_vkPipeline;

		vk::CommandPool m_commandPool;
		std::vector<vk::CommandBuffer> m_commandBuffers;

		int m_framesCount = 0;
		std::vector<vk::Semaphore> m_imageAvailableSemaphores;
		std::vector<vk::Semaphore> m_renderFinishedSemaphores;
		std::vector<vk::Fence> m_inFlightFences;
		uint32_t m_currentFrame = 0;
		uint32_t m_currentImageIndex = 0;

		bool m_stateBeginFrameSuccess = false;
	};


} // namespace Helios::Engine::Renderer::Vulkan
