//==============================================================================
// Vulkan Framebuffer Wrapper
//
// Encapsulates a vk::Framebuffer, which connects render pass attachments
// (like color and depth) to specific image views.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
//==============================================================================
#pragma once

namespace Helios::Engine::Renderer::Vulkan {


	class VKDeviceManager;

	class VKFramebuffer
	{
	public:
		VKFramebuffer(const VKDeviceManager& deviceManager, const vk::RenderPass& renderPass, const std::vector<vk::ImageView>& attachments, const vk::Extent2D& extent);
		~VKFramebuffer();

		// Prevent copying and moving
		VKFramebuffer(const VKFramebuffer&) = delete;
		VKFramebuffer& operator=(const VKFramebuffer&) = delete;
		VKFramebuffer(VKFramebuffer&&) = delete;
		VKFramebuffer& operator=(VKFramebuffer&&) = delete;

		const vk::Framebuffer& Get() const { return m_framebuffer; }

	private:
		vk::Framebuffer m_framebuffer = nullptr;

		// --- Dependencies ---
		const VKDeviceManager& m_deviceManager;
	};


} // namespace Helios::Engine::Renderer::Vulkan
