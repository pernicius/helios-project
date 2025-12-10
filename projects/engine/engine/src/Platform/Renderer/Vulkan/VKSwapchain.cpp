#include "pch.h"
#include "Platform/Renderer/Vulkan/VKSwapchain.h"

#include <stdexcept>
#include <algorithm>
#include <limits>

#include <GLFW/glfw3.h>

namespace Helios::Engine::Renderer::Vulkan {


	namespace { // Helpers ... used locally only


		struct SwapchainSupportDetails {
			vk::SurfaceCapabilitiesKHR capabilities{};
			std::vector<vk::SurfaceFormatKHR> formats{};
			std::vector<vk::PresentModeKHR> presentModes{};
		};


		SwapchainSupportDetails QuerySwapchainSupport(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface) {
			SwapchainSupportDetails details;
			details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
			details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
			details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
			return details;
		}


		vk::SurfaceFormatKHR ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
			for (const auto& avail : availableFormats) {
				if ((avail.format == vk::Format::eB8G8R8A8Unorm || avail.format == vk::Format::eR8G8B8A8Unorm) &&
					avail.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
					return avail;
				}
			}
			return availableFormats.empty() ? vk::SurfaceFormatKHR{} : availableFormats[0];
		}


		vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
			if (std::find(availablePresentModes.begin(), availablePresentModes.end(), vk::PresentModeKHR::eMailbox) != availablePresentModes.end()) {
				return vk::PresentModeKHR::eMailbox;
			}
			if (std::find(availablePresentModes.begin(), availablePresentModes.end(), vk::PresentModeKHR::eImmediate) != availablePresentModes.end()) {
				return vk::PresentModeKHR::eImmediate;
			}
			return vk::PresentModeKHR::eFifo;
		}


		vk::Extent2D ChooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
				return capabilities.currentExtent;
			}

			int width = 0, height = 0;
			glfwGetFramebufferSize(window, &width, &height);

			vk::Extent2D actualExtent{
				static_cast<uint32_t>(std::clamp(width, static_cast<int>(capabilities.minImageExtent.width), static_cast<int>(capabilities.maxImageExtent.width))),
				static_cast<uint32_t>(std::clamp(height, static_cast<int>(capabilities.minImageExtent.height), static_cast<int>(capabilities.maxImageExtent.height)))
			};
			return actualExtent;
		}


	} // namespace Helpers


	VKSwapchain::VKSwapchain(Ref<VKDevice>& device, vk::SurfaceKHR& surface, Ref<Window>& window)
		: m_Device(device), m_Surface(surface), m_Window(window)
	{
		Create();
	}


	VKSwapchain::~VKSwapchain()
	{
		Destroy();
	}


	void VKSwapchain::Create(vk::SwapchainKHR oldSwapchain)
	{
		LOG_RENDER_DEBUG("VKSwapchain: (Re-)Creating swapchain.");

		auto physical = m_Device->GetPhysicalDevice();
		auto device = m_Device->GetDevice();

		auto support = QuerySwapchainSupport(physical, m_Surface);
		if (support.formats.empty() || support.presentModes.empty()) {
			throw std::runtime_error("VKSwapchain: no supported formats or present modes.");
		}

		auto surfaceFormat = ChooseSurfaceFormat(support.formats);
		auto presentMode = ChoosePresentMode(support.presentModes);
		auto extent = ChooseExtent(support.capabilities, static_cast<GLFWwindow*>(m_Window->GetNativeWindow()));

		// image count
		uint32_t imageCount = support.capabilities.minImageCount + 1;
		if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
			imageCount = support.capabilities.maxImageCount;
		}

		// queue family indices
//		auto [graphicsIndex, presentIndex] = FindQueueFamilies(physical, m_Surface);
		uint32_t graphicsIndex = m_Device->GetGraphicsQueueIndex();
		uint32_t presentIndex = m_Device->GetPresentQueueIndex();
		uint32_t queueFamilyIndicesArr[2] = { graphicsIndex, presentIndex };

		vk::SwapchainCreateInfoKHR createInfo{};
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		if (graphicsIndex != presentIndex) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndicesArr;
		} else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		createInfo.preTransform = support.capabilities.currentTransform;

		// choose composite alpha
		std::vector<vk::CompositeAlphaFlagBitsKHR> compositePriority = {
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
			vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
			vk::CompositeAlphaFlagBitsKHR::eInherit
		};
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		for (auto c : compositePriority) {
			if ((support.capabilities.supportedCompositeAlpha & c) == c) {
				createInfo.compositeAlpha = c;
				break;
			}
		}

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = static_cast<VkSwapchainKHR>(oldSwapchain);

		// Create the new swapchain into a local handle first so we can keep the current swapchain alive
		// until new swapchain and views are created successfully.
		vk::SwapchainKHR newSwapchain = VK_NULL_HANDLE;
		auto resultCreate = device.createSwapchainKHR(&createInfo, nullptr, &newSwapchain);
		if (resultCreate != vk::Result::eSuccess) {
			throw std::runtime_error("VKSwapchain: failed to create swapchain.");
		}

		vk::Format newFormat = surfaceFormat.format;
		vk::Extent2D newExtent = extent;

		// retrieve images for new swapchain
		auto newImages = device.getSwapchainImagesKHR(newSwapchain);

		// create image views for new swapchain
		std::vector<vk::ImageView> newImageViews;
		newImageViews.resize(newImages.size());
		for (size_t i = 0; i < newImages.size(); ++i) {
			vk::ImageViewCreateInfo viewInfo{};
			viewInfo.image = newImages[i];
			viewInfo.viewType = vk::ImageViewType::e2D;
			viewInfo.format = newFormat;
			viewInfo.components.r = vk::ComponentSwizzle::eIdentity;
			viewInfo.components.g = vk::ComponentSwizzle::eIdentity;
			viewInfo.components.b = vk::ComponentSwizzle::eIdentity;
			viewInfo.components.a = vk::ComponentSwizzle::eIdentity;
			viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			auto res = device.createImageView(&viewInfo, nullptr, &newImageViews[i]);
			if (res != vk::Result::eSuccess) {
				// cleanup created new views and new swapchain, leave existing swapchain untouched
				for (size_t j = 0; j < i; ++j) device.destroyImageView(newImageViews[j]);
				device.destroySwapchainKHR(newSwapchain);
				throw std::runtime_error("VKSwapchain: failed to create image view for new swapchain.");
			}
		}

		// At this point new swapchain and its image views were created successfully.
		// Now destroy old image views and old swapchain (if provided).
		if (oldSwapchain != VK_NULL_HANDLE) {
			// destroy old image views
			for (auto& view : m_ImageViews) {
				if (view) device.destroyImageView(view);
			}
			m_ImageViews.clear();
			m_Images.clear();

			// destroy old swapchain handle
			if (m_Swapchain) {
				device.destroySwapchainKHR(m_Swapchain);

				LOG_RENDER_DEBUG("VKSwapchain: Destroyed old swapchain.");
			}
		}

		// Commit new swapchain
		m_Swapchain = newSwapchain;
		m_Images = std::move(newImages);
		m_ImageViews = std::move(newImageViews);
		m_Format = newFormat;
		m_Extent = newExtent;
	}


	void VKSwapchain::Destroy()
	{
		if (!m_Device) return;
		auto device = m_Device->GetDevice();

		device.waitIdle();

		for (auto& view : m_ImageViews) {
			if (view) device.destroyImageView(view);
		}
		m_ImageViews.clear();
		m_Images.clear();

		if (m_Swapchain) {
			device.destroySwapchainKHR(m_Swapchain);
			m_Swapchain = VK_NULL_HANDLE;
		}

		LOG_RENDER_DEBUG("VKSwapchain: Destroyed swapchain.");
	}


	void VKSwapchain::Recreate()
	{
		if (!m_Device) return;

		m_Device->GetDevice().waitIdle();

		if (m_Swapchain == VK_NULL_HANDLE) {
			Create();
			return;
		}

		// Create new swapchain using current swapchain as oldSwapchain ( will be destroyed in Create() )
		Create(m_Swapchain);
	}


} // namespace Helios::Engine::Renderer::Vulkan
