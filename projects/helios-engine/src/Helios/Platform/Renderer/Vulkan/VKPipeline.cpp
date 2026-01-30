//==============================================================================
// Vulkan Graphics Pipeline (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VKPipeline.h
//==============================================================================
#include "pch.h"
#include "VKPipeline.h"

#include "Helios/Engine/VFS/VFS.h"

namespace Helios::Engine::Renderer::Vulkan {


	//------------------------------------------------------------------------------
	// VKPipeline Implementation
	//------------------------------------------------------------------------------

	VKPipeline::VKPipeline(const VKDeviceManager& deviceManager, const VKPipelineBuilder& builder)
		: m_deviceManager(deviceManager)
	{
		m_pipelineLayout = m_deviceManager.GetLogicalDevice().createPipelineLayout(builder.m_pipelineLayoutInfo);
		LOG_RENDER_DEBUG("VKPipeline: PipelineLayout created.");

		vk::GraphicsPipelineCreateInfo pipelineInfo = vk::GraphicsPipelineCreateInfo()
			.setStageCount(static_cast<uint32_t>(builder.m_shaderStages.size()))
			.setPStages(builder.m_shaderStages.data())
			.setPVertexInputState(&builder.m_vertexInputInfo)
			.setPInputAssemblyState(&builder.m_inputAssembly)
			.setPViewportState(&builder.m_viewportState)
			.setPRasterizationState(&builder.m_rasterizer)
			.setPMultisampleState(&builder.m_multisampling)
			.setPDepthStencilState(&builder.m_depthStencil)
			.setPColorBlendState(&builder.m_colorBlending)
			.setPDynamicState(&builder.m_dynamicState)
			.setLayout(m_pipelineLayout)
			.setRenderPass(builder.m_renderPass.Get())
			.setSubpass(0) // Index of the subpass where this pipeline will be used
			.setBasePipelineHandle(nullptr)
			.setBasePipelineIndex(-1);

		auto result = m_deviceManager.GetLogicalDevice().createGraphicsPipeline(nullptr, pipelineInfo);
		if (result.result != vk::Result::eSuccess) {
			LOG_RENDER_EXCEPT("VKPipeline: Failed to create graphics pipeline!");
		}
		m_pipeline = result.value;
		LOG_RENDER_DEBUG("VKPipeline: Graphics pipeline created.");
	}


	VKPipeline::~VKPipeline()
	{
		if (m_pipeline) {
			m_deviceManager.GetLogicalDevice().destroyPipeline(m_pipeline);
			m_pipeline = nullptr;
			LOG_RENDER_DEBUG("VKPipeline: Graphics pipeline destroyed.");
		}
		if (m_pipelineLayout) {
			m_deviceManager.GetLogicalDevice().destroyPipelineLayout(m_pipelineLayout);
			m_pipelineLayout = nullptr;
			LOG_RENDER_DEBUG("VKPipeline: PipelineLayout destroyed.");
		}
	}


	//------------------------------------------------------------------------------
	// VKPipelineBuilder Implementation
	//------------------------------------------------------------------------------

	VKPipelineBuilder::VKPipelineBuilder(const VKDeviceManager& deviceManager, const VKRenderPass& renderPass)
		: m_deviceManager(deviceManager), m_renderPass(renderPass)
	{
		// Default states
		m_inputAssembly = vk::PipelineInputAssemblyStateCreateInfo()
			.setTopology(vk::PrimitiveTopology::eTriangleList)
			.setPrimitiveRestartEnable(VK_FALSE);

		m_viewportState = vk::PipelineViewportStateCreateInfo()
			.setViewportCount(1)
			.setScissorCount(1);

		m_rasterizer = vk::PipelineRasterizationStateCreateInfo()
			.setDepthClampEnable(VK_FALSE)
			.setRasterizerDiscardEnable(VK_FALSE)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setLineWidth(1.0f)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eClockwise)
			.setDepthBiasEnable(VK_FALSE);

		m_multisampling = vk::PipelineMultisampleStateCreateInfo()
			.setSampleShadingEnable(VK_FALSE)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);

		m_colorBlending = vk::PipelineColorBlendStateCreateInfo()
			.setLogicOpEnable(VK_FALSE)
			.setLogicOp(vk::LogicOp::eCopy);

		m_depthStencil = vk::PipelineDepthStencilStateCreateInfo();

		m_pipelineLayoutInfo = vk::PipelineLayoutCreateInfo();

		m_dynamicState = vk::PipelineDynamicStateCreateInfo();
	}


	VKPipelineBuilder::~VKPipelineBuilder()
	{
		for (auto& shaderStage : m_shaderStages) {
			if (shaderStage.module) {
				m_deviceManager.GetLogicalDevice().destroyShaderModule(shaderStage.module);
			}
		}
	}


	VKPipelineBuilder& VKPipelineBuilder::SetShaders(const std::string& vertPath, const std::string& fragPath)
	{
		vk::ShaderModule vertShaderModule = CreateShaderModule(vertPath);
		vk::ShaderModule fragShaderModule = CreateShaderModule(fragPath);

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(vertShaderModule)
			.setPName("main");

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(fragShaderModule)
			.setPName("main");

		m_shaderStages = { vertShaderStageInfo, fragShaderStageInfo };
		return *this;
	}


	VKPipelineBuilder& VKPipelineBuilder::SetVertexInput(
		const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions,
		const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions)
	{
		m_bindingDescriptions = bindingDescriptions;
		m_attributeDescriptions = attributeDescriptions;
		return *this;
	}


	VKPipelineBuilder& VKPipelineBuilder::SetInputAssembly(vk::PrimitiveTopology topology, vk::Bool32 primitiveRestartEnable)
	{
		m_inputAssembly.setTopology(topology);
		m_inputAssembly.setPrimitiveRestartEnable(primitiveRestartEnable);
		return *this;
	}


	VKPipelineBuilder& VKPipelineBuilder::SetViewport(const vk::Viewport& viewport)
	{
		m_viewportState.setPViewports(&viewport);
		return *this;
	}


	VKPipelineBuilder& VKPipelineBuilder::SetScissor(const vk::Rect2D& scissor)
	{
		m_viewportState.setPScissors(&scissor);
		return *this;
	}


	VKPipelineBuilder& VKPipelineBuilder::SetRasterizer(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace)
	{
		m_rasterizer.setPolygonMode(polygonMode);
		m_rasterizer.setCullMode(cullMode);
		m_rasterizer.setFrontFace(frontFace);
		return *this;
	}


	VKPipelineBuilder& VKPipelineBuilder::SetMultisampling(vk::SampleCountFlagBits rasterizationSamples)
	{
		m_multisampling.setRasterizationSamples(rasterizationSamples);
		return *this;
	}


	VKPipelineBuilder& VKPipelineBuilder::SetColorBlending(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment, vk::Bool32 logicOpEnable, vk::LogicOp logicOp)
	{
		m_colorBlendAttachments.push_back(colorBlendAttachment);
		m_colorBlending.setLogicOpEnable(logicOpEnable);
		m_colorBlending.setLogicOp(logicOp);
		return *this;
	}


	VKPipelineBuilder& VKPipelineBuilder::SetDepthStencil(vk::Bool32 depthTestEnable, vk::Bool32 depthWriteEnable, vk::CompareOp depthCompareOp)
	{
		m_depthStencil.setDepthTestEnable(depthTestEnable);
		m_depthStencil.setDepthWriteEnable(depthWriteEnable);
		m_depthStencil.setDepthCompareOp(depthCompareOp);
		return *this;
	}


	VKPipelineBuilder& VKPipelineBuilder::SetDynamicState(const std::vector<vk::DynamicState>& dynamicStates)
	{
		m_dynamicStates = dynamicStates;
		return *this;
	}


	Scope<VKPipeline> VKPipelineBuilder::Build()
	{
		m_vertexInputInfo = vk::PipelineVertexInputStateCreateInfo()
			.setVertexBindingDescriptionCount(static_cast<uint32_t>(m_bindingDescriptions.size()))
			.setPVertexBindingDescriptions(m_bindingDescriptions.data())
			.setVertexAttributeDescriptionCount(static_cast<uint32_t>(m_attributeDescriptions.size()))
			.setPVertexAttributeDescriptions(m_attributeDescriptions.data());

		m_colorBlending.setAttachmentCount(static_cast<uint32_t>(m_colorBlendAttachments.size()));
		m_colorBlending.setPAttachments(m_colorBlendAttachments.data());

		m_dynamicState.setDynamicStateCount(static_cast<uint32_t>(m_dynamicStates.size()));
		m_dynamicState.setPDynamicStates(m_dynamicStates.data());

		return CreateScope<VKPipeline>(m_deviceManager, *this);
	}


	vk::ShaderModule VKPipelineBuilder::CreateShaderModule(const std::string& path)
	{
		auto shaderBinary = VirtFS.ReadBinary(path);
		if (shaderBinary.empty()) {
			LOG_RENDER_EXCEPT("VKPipeline: Failed to load shader: '{}'", path);
		}
		std::vector<char> code(shaderBinary.begin(), shaderBinary.end());

		vk::ShaderModuleCreateInfo createInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(code.size())
			.setPCode(reinterpret_cast<const uint32_t*>(code.data()));

		vk::ShaderModule shaderModule = m_deviceManager.GetLogicalDevice().createShaderModule(createInfo);
		LOG_RENDER_DEBUG("VKPipeline: Shader module created for '{}'.", path);
		return shaderModule;
	}


} // namespace Helios::Engine::Renderer::Vulkan
