//==============================================================================
// Vulkan Graphics Pipeline
//
// Encapsulates a Vulkan graphics pipeline and its layout. This file provides a
// `VKPipeline` class for RAII-style management and a `VKPipelineBuilder` for
// flexible construction of complex pipeline states.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - `VKPipeline`: RAII wrapper for `vk::Pipeline` and `vk::PipelineLayout`.
// - `VKPipelineBuilder`: A builder pattern for constructing graphics pipelines.
// - Configures all programmable and fixed-function stages.
// - Handles shader module creation from SPIR-V code.
//
// Changelog:
// - 2026.01: Initial version
//==============================================================================
#pragma once

#include "Helios/Platform/Renderer/Vulkan/VKDeviceManager.h"
#include "Helios/Platform/Renderer/Vulkan/VKRenderPass.h"

namespace Helios::Engine::Renderer::Vulkan {


	class VKPipelineBuilder;


	class VKPipeline
	{
	public:
		VKPipeline(const VKDeviceManager& deviceManager, const VKPipelineBuilder& builder);
		~VKPipeline();

		// Prevent copying and moving
		VKPipeline(const VKPipeline&) = delete;
		VKPipeline& operator=(const VKPipeline&) = delete;
		VKPipeline(VKPipeline&&) = delete;
		VKPipeline& operator=(VKPipeline&&) = delete;

		vk::Pipeline GetPipeline() const { return m_pipeline; }
		vk::PipelineLayout GetLayout() const { return m_pipelineLayout; }

	private:
		const VKDeviceManager& m_deviceManager;
		vk::Pipeline m_pipeline = nullptr;
		vk::PipelineLayout m_pipelineLayout = nullptr;
	};


	//------------------------------------------------------------------------------
	// VKPipelineBuilder
	//------------------------------------------------------------------------------
	class VKPipelineBuilder
	{
	public:
		VKPipelineBuilder(const VKDeviceManager& deviceManager, const VKRenderPass& renderPass);
		~VKPipelineBuilder();

		VKPipelineBuilder& SetShaders(const std::string& vertPath, const std::string& fragPath);
		VKPipelineBuilder& SetVertexInput(
			const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions,
			const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions);
		VKPipelineBuilder& SetInputAssembly(vk::PrimitiveTopology topology, vk::Bool32 primitiveRestartEnable = VK_FALSE);
		VKPipelineBuilder& SetViewport(const vk::Viewport& viewport);
		VKPipelineBuilder& SetScissor(const vk::Rect2D& scissor);
		VKPipelineBuilder& SetRasterizer(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace);
		VKPipelineBuilder& SetMultisampling(vk::SampleCountFlagBits rasterizationSamples = vk::SampleCountFlagBits::e1);
		VKPipelineBuilder& SetColorBlending(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment, vk::Bool32 logicOpEnable = VK_FALSE, vk::LogicOp logicOp = vk::LogicOp::eCopy);
		VKPipelineBuilder& SetDepthStencil(vk::Bool32 depthTestEnable, vk::Bool32 depthWriteEnable, vk::CompareOp depthCompareOp);
		VKPipelineBuilder& SetDynamicState(const std::vector<vk::DynamicState>& dynamicStates);

		Scope<VKPipeline> Build();

	private:
		friend class VKPipeline;

		vk::ShaderModule CreateShaderModule(const std::string& path);

		const VKDeviceManager& m_deviceManager;
		const VKRenderPass& m_renderPass;

		std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages;
		vk::PipelineVertexInputStateCreateInfo m_vertexInputInfo;
		vk::PipelineInputAssemblyStateCreateInfo m_inputAssembly;
		vk::PipelineViewportStateCreateInfo m_viewportState;
		vk::PipelineRasterizationStateCreateInfo m_rasterizer;
		vk::PipelineMultisampleStateCreateInfo m_multisampling;
		vk::PipelineColorBlendStateCreateInfo m_colorBlending;
		vk::PipelineDepthStencilStateCreateInfo m_depthStencil;
		vk::PipelineDynamicStateCreateInfo m_dynamicState;
		vk::PipelineLayoutCreateInfo m_pipelineLayoutInfo;

		std::vector<vk::VertexInputBindingDescription> m_bindingDescriptions;
		std::vector<vk::VertexInputAttributeDescription> m_attributeDescriptions;
		std::vector<vk::PipelineColorBlendAttachmentState> m_colorBlendAttachments;
		std::vector<vk::DynamicState> m_dynamicStates;
	};


} // namespace Helios::Engine::Renderer::Vulkan
