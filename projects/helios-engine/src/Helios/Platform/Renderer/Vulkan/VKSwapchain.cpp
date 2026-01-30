//==============================================================================
// Vulkan Surface Wrapper (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VKSurface.h
//==============================================================================
#include "pch.h"
#include "VKSwapchain.h"

#include "Helios/Platform/Renderer/Vulkan/VKDeviceManager.h"
#include "Helios/Platform/Renderer/Vulkan/VKSurface.h"

namespace Helios::Engine::Renderer::Vulkan {


	VKSwapchain::VKSwapchain(const VKDeviceManager& deviceManager, const VKSurface& surface, Window& window)
		: m_deviceManager(deviceManager), m_surface(surface)
	{
		CreateSwapchain(window);
		CreateImageViews();
	}

	
	VKSwapchain::~VKSwapchain()
	{
		Cleanup();
	}


	void VKSwapchain::Cleanup()
	{
		if (!m_framebuffers.empty()) {
			m_framebuffers.clear();
			LOG_RENDER_DEBUG("VKSwapchain: Framebuffers destroyed.");
		}

		const vk::Device& logicalDevice = m_deviceManager.GetLogicalDevice();

		if (m_imageViews.size() > 0 && m_imageViews[0]) {
			for (auto imageView : m_imageViews)
				logicalDevice.destroyImageView(imageView);
			m_imageViews.clear();
			LOG_RENDER_DEBUG("VKSwapchain: ImageViews destroyed.");
		}


		if (m_swapchain) {
			logicalDevice.destroySwapchainKHR(m_swapchain);
			m_swapchain = nullptr;
			LOG_RENDER_DEBUG("VKSwapchain: Swapchain destroyed.");
		}
	}


	void VKSwapchain::Recreate(Window& window, const vk::RenderPass& renderPass)
	{
		// Handle minimization (only recreate when not minimized)
		if (window.glfwIsMinimized())
			return;

		// Wait for the device to be idle before recreating resources
		m_deviceManager.GetLogicalDevice().waitIdle();

		// Clean up old resources before creating new ones
		Cleanup();

		// Recreate the swapchain and its image views
		CreateSwapchain(window);
		CreateImageViews();
		CreateFramebuffers(renderPass);
	}


	void VKSwapchain::CreateFramebuffers(const vk::RenderPass& renderPass)
	{
		m_framebuffers.resize(m_imageViews.size());

		for (size_t i = 0; i < m_imageViews.size(); i++) {
			std::vector<vk::ImageView> attachments = { m_imageViews[i] };
			m_framebuffers[i] = CreateScope<VKFramebuffer>(m_deviceManager, renderPass, attachments, m_extent);
		}
	}


	void VKSwapchain::CreateSwapchain(Window& window)
	{
		const vk::PhysicalDevice physicalDevice = m_deviceManager.GetPhysicalDevice();
		const vk::Device& logicalDevice = m_deviceManager.GetLogicalDevice();
		SwapchainSupportDetails support = QuerySupport(physicalDevice);

		vk::SurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(support.formats);
		vk::PresentModeKHR presentMode = ChoosePresentMode(support.presentModes);
		vk::Extent2D extent = ChooseExtent(support.capabilities, window);

		uint32_t imageCount = support.capabilities.minImageCount + 1;
		if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
			imageCount = support.capabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR()
			.setSurface(m_surface.Get())
			.setMinImageCount(imageCount)
			.setImageFormat(surfaceFormat.format)
			.setImageColorSpace(surfaceFormat.colorSpace)
			.setImageExtent(extent)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

		QueueFamilyIndices indices = m_deviceManager.GetQueueFamilyIndices();
		uint32_t queueFamilyIndices[] = { indices.graphics.value(), indices.present.value() };

		if (indices.graphics != indices.present) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		createInfo.preTransform = support.capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = nullptr;

		m_swapchain = logicalDevice.createSwapchainKHR(createInfo);
		LOG_RENDER_DEBUG("VKSwapchain: Swapchain created.");

		// Store properties for later use
		m_images = logicalDevice.getSwapchainImagesKHR(m_swapchain);
		m_imageFormat = surfaceFormat.format;
		m_extent = extent;
	}


	void VKSwapchain::CreateImageViews()
	{
		m_imageViews.resize(m_images.size());

		vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1);

		for (size_t i = 0; i < m_images.size(); i++) {
			vk::ImageViewCreateInfo createInfo = vk::ImageViewCreateInfo()
				.setImage(m_images[i])
				.setViewType(vk::ImageViewType::e2D)
				.setFormat(m_imageFormat)
				.setSubresourceRange(subresourceRange);
			m_imageViews[i] = m_deviceManager.GetLogicalDevice().createImageView(createInfo);
		}
		LOG_RENDER_DEBUG("VKSwapchain: ImageViews created.");

	}


	SwapchainSupportDetails VKSwapchain::QuerySupport(vk::PhysicalDevice physicalDevice) const
	{
		SwapchainSupportDetails details;
		const vk::SurfaceKHR& surface = m_surface.Get();

		details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
		details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
		details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

		return details;
	}


	vk::SurfaceFormatKHR VKSwapchain::ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return availableFormat;
			}
		}
		return availableFormats[0];
	}


	vk::PresentModeKHR VKSwapchain::ChoosePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes) {
#if 0
			// VSync On
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			}
#else
			// VSync Off
			if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
				return availablePresentMode;
			}
#endif
		}
		return vk::PresentModeKHR::eFifo;
	}


	vk::Extent2D VKSwapchain::ChooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, Window& window) const
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(static_cast<GLFWwindow*>(window.GetNativeWindow()), &width, &height);

			vk::Extent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}


} // namespace Helios::Engine::Renderer::Vulkan

