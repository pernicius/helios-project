#pragma once

#include "Platform/Renderer/Vulkan/VKWindow.h"
#include "Platform/Renderer/Vulkan/VKDevice.h"

#include <vulkan/vulkan.hpp>

namespace Helios::Engine::Renderer::Vulkan {


	class VKSwapchain
	{
	public:
		VKSwapchain() = delete;
		VKSwapchain(Ref<VKDevice>& device, vk::SurfaceKHR& surface, Ref<Window>& window);
		~VKSwapchain();

		// Non-copyable
		VKSwapchain(const VKSwapchain&) = delete;
		VKSwapchain& operator=(const VKSwapchain&) = delete;

		// Accessors
		const vk::SwapchainKHR& GetSwapchain() const { return m_Swapchain; }
		const std::vector<vk::Image>& GetImages() const { return m_Images; }
		const std::vector<vk::ImageView>& GetImageViews() const { return m_ImageViews; }
		vk::Format GetFormat() const { return m_Format; }
		vk::Extent2D GetExtent() const { return m_Extent; }

		// Recreate swapchain (useful on resize). Caller should ensure device is idle before destroying old resources if needed.
		void Recreate();

	private:
		void Create();
		void Destroy();

	private:
		Ref<VKDevice> m_Device{};
		vk::SurfaceKHR m_Surface{};
		Ref<Window> m_Window{};

		vk::SwapchainKHR m_Swapchain{ VK_NULL_HANDLE };
		std::vector<vk::Image> m_Images;
		std::vector<vk::ImageView> m_ImageViews;
		vk::Format m_Format{};
		vk::Extent2D m_Extent{};
	};


} // namespace Helios::Engine::Renderer::Vulkan
