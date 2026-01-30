//==============================================================================
// Vulkan Framebuffer Wrapper (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VKFramebuffer.h
//==============================================================================
#include "pch.h"
#include "VKFramebuffer.h"

#include "Helios/Platform/Renderer/Vulkan/VKDeviceManager.h"

namespace Helios::Engine::Renderer::Vulkan {


	VKFramebuffer::VKFramebuffer(const VKDeviceManager& deviceManager, const vk::RenderPass& renderPass, const std::vector<vk::ImageView>& attachments, const vk::Extent2D& extent)
		: m_deviceManager(deviceManager)
	{
		vk::FramebufferCreateInfo framebufferInfo = vk::FramebufferCreateInfo()
			.setRenderPass(renderPass)
			.setAttachmentCount(static_cast<uint32_t>(attachments.size()))
			.setPAttachments(attachments.data())
			.setWidth(extent.width)
			.setHeight(extent.height)
			.setLayers(1);

		m_framebuffer = m_deviceManager.GetLogicalDevice().createFramebuffer(framebufferInfo);
		LOG_RENDER_DEBUG("VKFramebuffer: Framebuffer created.");
	}


	VKFramebuffer::~VKFramebuffer()
	{
		if (m_framebuffer) {
			m_deviceManager.GetLogicalDevice().destroyFramebuffer(m_framebuffer);
			m_framebuffer = nullptr;
			LOG_RENDER_DEBUG("VKFramebuffer: Framebuffer destroyed.");
		}
	}


} // namespace Helios::Engine::Renderer::Vulkan
