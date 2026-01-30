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

	VKRenderPass::VKRenderPass(const VKDeviceManager& deviceManager, const VKRenderPassBuilder& builder)
		: m_deviceManager(deviceManager),
		m_attachments(builder.m_attachments),
		m_subpasses(builder.m_subpasses),
		m_dependencies(builder.m_dependencies),
		m_subpassAttachmentRefs(builder.m_subpassAttachmentRefs)
	{
		// The builder stores attachment references, but the subpass descriptions
		// need valid pointers. We must repoint them to the stored vectors.
		for (size_t i = 0; i < m_subpasses.size(); ++i) {
			if (!m_subpassAttachmentRefs[i].empty()) {
				m_subpasses[i].pColorAttachments = m_subpassAttachmentRefs[i].data();
			}
			if (m_subpasses[i].pDepthStencilAttachment) {
				// This assumes one depth attachment per subpass for simplicity.
				// The builder stores depth refs separately.
				m_subpasses[i].pDepthStencilAttachment = &builder.m_depthAttachmentRefs[i];
			}
		}

		vk::RenderPassCreateInfo renderPassInfo = vk::RenderPassCreateInfo()
			.setAttachmentCount(static_cast<uint32_t>(m_attachments.size()))
			.setPAttachments(m_attachments.data())
			.setSubpassCount(static_cast<uint32_t>(m_subpasses.size()))
			.setPSubpasses(m_subpasses.data())
			.setDependencyCount(static_cast<uint32_t>(m_dependencies.size()))
			.setPDependencies(m_dependencies.data());

		m_renderPass = m_deviceManager.GetLogicalDevice().createRenderPass(renderPassInfo);
		LOG_RENDER_DEBUG("VKRenderPass: RenderPass created.");
	}


	VKRenderPass::~VKRenderPass()
	{
		if (m_renderPass) {
			m_deviceManager.GetLogicalDevice().destroyRenderPass(m_renderPass);
			m_renderPass = nullptr;
			LOG_RENDER_DEBUG("VKRenderPass: RenderPass destroyed.");
		}
	}


	//------------------------------------------------------------------------------
	// VKRenderPassBuilder Implementation
	//------------------------------------------------------------------------------
	
	VKRenderPassBuilder::VKRenderPassBuilder(const VKDeviceManager& deviceManager)
		: m_deviceManager(deviceManager)
	{
	}


	VKRenderPassBuilder& VKRenderPassBuilder::AddAttachment(const vk::AttachmentDescription& attachment)
	{
		m_attachments.push_back(attachment);
		return *this;
	}


	VKRenderPassBuilder& VKRenderPassBuilder::AddSubpass(const std::vector<vk::AttachmentReference>& colorAttachments, const vk::AttachmentReference* depthAttachment)
	{
		// Store a copy of the attachment references for this subpass
		m_subpassAttachmentRefs.push_back(colorAttachments);

		vk::SubpassDescription subpass = vk::SubpassDescription()
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachmentCount(static_cast<uint32_t>(colorAttachments.size()))
			.setPColorAttachments(nullptr); // Will be set in VKRenderPass constructor

		if (depthAttachment) {
			m_depthAttachmentRefs.push_back(*depthAttachment);
			subpass.pDepthStencilAttachment = nullptr; // Will be set in VKRenderPass constructor
		} else {
			// Add a placeholder to keep indices in sync
			m_depthAttachmentRefs.emplace_back();
		}

		m_subpasses.push_back(subpass);
		return *this;
	}


	VKRenderPassBuilder& VKRenderPassBuilder::AddDependency(const vk::SubpassDependency& dependency)
	{
		m_dependencies.push_back(dependency);
		return *this;
	}


	Scope<VKRenderPass> VKRenderPassBuilder::Build()
	{
		return CreateScope<VKRenderPass>(m_deviceManager, *this);
	}


} // namespace Helios::Engine::Renderer::Vulkan
