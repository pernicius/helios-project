//==============================================================================
// Vulkan Render Pass (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VKRenderPass.h
//==============================================================================
#include "pch.h"
#include "VKRenderPass.h"

namespace Helios::Engine::Renderer::Vulkan {


	//------------------------------------------------------------------------------
	// VKRenderPass Implementation
	//------------------------------------------------------------------------------
	VKRenderPass::VKRenderPass(const VKDeviceManager& deviceManager, const VKSwapchain& swapchain)
		: m_DeviceManager(deviceManager)
	{
		vk::AttachmentDescription colorAttachment = vk::AttachmentDescription()
			.setFormat(swapchain.GetImageFormat())
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

		vk::AttachmentReference colorAttachmentRef = vk::AttachmentReference()
			.setAttachment(0)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		vk::SubpassDescription subpass = vk::SubpassDescription()
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachmentCount(1)
			.setPColorAttachments(&colorAttachmentRef);

		vk::SubpassDependency dependency = vk::SubpassDependency()
			.setSrcSubpass(VK_SUBPASS_EXTERNAL)
			.setDstSubpass(0)
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setSrcAccessMask({})
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

		vk::RenderPassCreateInfo renderPassInfo = vk::RenderPassCreateInfo()
			.setAttachmentCount(1)
			.setPAttachments(&colorAttachment)
			.setSubpassCount(1)
			.setPSubpasses(&subpass)
			.setDependencyCount(1)
			.setPDependencies(&dependency);

		m_RenderPass = m_DeviceManager.GetLogicalDevice().createRenderPass(renderPassInfo);
		LOG_RENDER_DEBUG("VKRenderPass: RenderPass created.");
	}

	VKRenderPass::~VKRenderPass()
	{
		if (m_RenderPass) {
			m_DeviceManager.GetLogicalDevice().destroyRenderPass(m_RenderPass);
			m_RenderPass = nullptr;
			LOG_RENDER_DEBUG("VKRenderPass: RenderPass destroyed.");
		}
	}


} // namespace Helios::Engine::Renderer::Vulkan
