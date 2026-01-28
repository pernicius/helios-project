//==============================================================================
// Vulkan Swapchain Wrapper
//
// Manages the Vulkan swapchain, which is a queue of images that are waiting
// to be presented to the screen. This class is responsible for creating the
// swapchain, retrieving its images, and creating the necessary image views.
// It also handles swapchain recreation when the window is resized.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
//
// Main Features:
// - Creates and manages a vk::SwapchainKHR object.
// - Queries for and selects optimal surface format, present mode, and extent.
// - Creates vk::ImageViews for each swapchain image.
// - Provides accessors for swapchain properties.
//
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

namespace Helios::Engine::Renderer::Vulkan {


	class VKDeviceManager;
	class VKSurface;


	struct SwapchainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};


	class VKSwapchain
	{
	public:
		VKSwapchain(const VKDeviceManager& deviceManager, const VKSurface& surface, Window& window);
		~VKSwapchain();

		// Prevent copying and moving
		VKSwapchain(const VKSwapchain&) = delete;
		VKSwapchain& operator=(const VKSwapchain&) = delete;
		VKSwapchain(VKSwapchain&&) = delete;
		VKSwapchain& operator=(VKSwapchain&&) = delete;

		void RecreateSwapchain(Window& window);

		// --- Accessors ---
		const vk::SwapchainKHR& GetSwapchain() const { return m_swapchain; }
		const std::vector<vk::Image>& GetImages() const { return m_images; }
		const std::vector<vk::ImageView>& GetImageViews() const { return m_imageViews; }
		const vk::Format& GetImageFormat() const { return m_imageFormat; }
		const vk::Extent2D& GetExtent() const { return m_extent; }

	private:
		// --- Initialization Steps ---
		void CreateSwapchain(Window& window);
		void CreateImageViews();
		void CleanupSwapchain();

		// --- Helper Functions ---
		SwapchainSupportDetails QuerySupport(vk::PhysicalDevice physicalDevice) const;
		vk::SurfaceFormatKHR ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
		vk::Extent2D ChooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, Window& window) const;

	private:
		vk::SwapchainKHR m_swapchain = nullptr;
		std::vector<vk::Image> m_images;
		std::vector<vk::ImageView> m_imageViews;
		vk::Format m_imageFormat;
		vk::Extent2D m_extent;

		// --- Dependencies ---
		const VKDeviceManager& m_deviceManager;
		const VKSurface& m_surface;
	};


} // namespace Helios::Engine::Renderer::Vulkan
