//==============================================================================
// Vulkan Render Pass
//
// Encapsulates a Vulkan render pass, which describes the attachments,
// subpasses, and dependencies required for rendering operations. This class
// defines the structure of a rendering process, including how color and depth
// buffers are handled.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// - Creates a vk::RenderPass with a single color attachment.
// - Configures a subpass for graphics rendering.
// - Sets up dependencies to ensure correct image layout transitions.
//
// Version history:
// - 2026.01: Initial version
//==============================================================================
#pragma once

#include "Helios/Platform/Renderer/Vulkan/VKDeviceManager.h"
#include "Helios/Platform/Renderer/Vulkan/VKSwapchain.h"

namespace Helios::Engine::Renderer::Vulkan {


	class VKRenderPass
	{
	public:
		VKRenderPass(const VKDeviceManager& deviceManager, const VKSwapchain& swapchain);
		~VKRenderPass();

		vk::RenderPass GetRenderPass() const { return m_RenderPass; }

	private:
		const VKDeviceManager& m_DeviceManager;
		vk::RenderPass m_RenderPass = nullptr;
	};


} // namespace Helios::Engine::Renderer::Vulkan
