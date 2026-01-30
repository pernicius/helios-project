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
// - 2026.01: Refactored to use a builder pattern
// - 2026.01: Initial version
//==============================================================================
#pragma once

#include "Helios/Platform/Renderer/Vulkan/VKDeviceManager.h"

namespace Helios::Engine::Renderer::Vulkan {


	class VKRenderPassBuilder;


	class VKRenderPass
	{
	public:
		VKRenderPass(const VKDeviceManager& deviceManager, const VKRenderPassBuilder& builder);
		~VKRenderPass();

		// Prevent copying and moving
		VKRenderPass(const VKRenderPass&) = delete;
		VKRenderPass& operator=(const VKRenderPass&) = delete;
		VKRenderPass(VKRenderPass&&) = delete;
		VKRenderPass& operator=(VKRenderPass&&) = delete;

		vk::RenderPass GetRenderPass() const { return m_renderPass; }

	private:
		friend class VKRenderPassBuilder;

		const VKDeviceManager& m_deviceManager;
		vk::RenderPass m_renderPass = nullptr;

		std::vector<vk::AttachmentDescription> m_attachments;
		std::vector<vk::SubpassDescription> m_subpasses;
		std::vector<vk::SubpassDependency> m_dependencies;
		std::vector<std::vector<vk::AttachmentReference>> m_subpassAttachmentRefs; // To hold persistent references
	};


	//------------------------------------------------------------------------------
	// VKRenderPassBuilder
	//------------------------------------------------------------------------------
	class VKRenderPassBuilder
	{
	public:
		VKRenderPassBuilder(const VKDeviceManager& deviceManager);

		VKRenderPassBuilder& AddAttachment(const vk::AttachmentDescription& attachment);
		VKRenderPassBuilder& AddSubpass(const std::vector<vk::AttachmentReference>& colorAttachments, const vk::AttachmentReference* depthAttachment = nullptr);
		VKRenderPassBuilder& AddDependency(const vk::SubpassDependency& dependency);

		Scope<VKRenderPass> Build();

	private:
		friend class VKRenderPass;

		const VKDeviceManager& m_deviceManager;
		std::vector<vk::AttachmentDescription> m_attachments;
		std::vector<std::vector<vk::AttachmentReference>> m_subpassAttachmentRefs;
		std::vector<vk::AttachmentReference> m_depthAttachmentRefs;
		std::vector<vk::SubpassDescription> m_subpasses;
		std::vector<vk::SubpassDependency> m_dependencies;
	};


} // namespace Helios::Engine::Renderer::Vulkan
