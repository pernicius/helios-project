#pragma once

#include "Platform/Renderer/Vulkan/VKInstance.h"
#include "Platform/Renderer/Vulkan/Misc.h"

#include <vulkan/vulkan.hpp>

namespace Helios::Engine::Renderer::Vulkan
{


	class VKDevice
	{
	public:
		VKDevice() = delete;
		VKDevice(Ref<VKInstance> instance, ExtensionStruct& extensions, vk::SurfaceKHR surface);
		~VKDevice();

	public:
		// accessorss, overloads, and conversions
		const vk::Device& GetDevice() const { return m_LogicalDevice; }
		VkDevice GetVkDevice() const noexcept { return static_cast<VkDevice>(m_LogicalDevice); }
		const vk::PhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
		VkPhysicalDevice GetVkPhysicalDevice() const noexcept { return static_cast<VkPhysicalDevice>(m_PhysicalDevice); }
		const vk::SurfaceKHR& GetSurface() const { return m_Surface; }
		VkSurfaceKHR GetVkSurface() const noexcept { return static_cast<VkSurfaceKHR>(m_Surface); }

		const vk::detail::DispatchLoaderDynamic& GetDispatchLoader() const { return m_DispatchLoader; }
		
		const ExtensionStruct& GetExtensions() const { return m_Extensions.device; }

		// queues
		const vk::Queue& GetGraphicsQueue() const { return m_Queues.graphics; }
		const vk::Queue& GetPresentQueue() const { return m_Queues.present; }
		const vk::Queue& GetComputeQueue() const { return m_Queues.compute; }
		const vk::Queue& GetTransferQueue() const { return m_Queues.transfer; }

		// device helpers
//		void WaitIdle();

		// convenience resource-creation helpers
//		vk::UniqueCommandPool CreateCommandPool(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags flags = {}) const;
//		std::vector<vk::UniqueCommandBuffer> AllocateCommandBuffers(const vk::CommandPool& pool, vk::CommandBufferLevel level, uint32_t count) const;
//		vk::UniqueFence CreateFence(vk::FenceCreateFlags flags = {}) const;
//		vk::UniqueSemaphore CreateSemaphore(vk::SemaphoreCreateFlags flags = {}) const;

	private:
		// simple, configurable criteria used when choosing a physical device.
		struct DeviceSelectionCriteria
		{
			// require swapchain support (useful when creating a present-capable device).
			// default true for typical windowed applications.
			bool requireSwapchain = true;

			// prefer discrete GPUs when scoring.
			bool preferDiscrete = true;

			// weights used while scoring devices (tweakable).
			int discreteWeight = 1000;
			int imageDimWeight = 1;            // multiplier for maxImageDimension2D / 1024
			int optionalExtensionWeight = 50;  // bonus per optional extension present

			DeviceSelectionCriteria() = default;
		};

		struct QueueFamilyInfo
		{
			std::optional<uint32_t> index;
			vk::QueueFlags flags = {};
			uint32_t queueCount = 0;
			bool presentSupport = false;
			bool dedicatedCompute = false;
			bool dedicatedTransfer = false;
			uint32_t timestampValidBits = 0;
			int heuristicScore = 0;
		};

		struct QueueFamilyIndices
		{
			QueueFamilyInfo graphics;
			QueueFamilyInfo present;
			QueueFamilyInfo compute;
			QueueFamilyInfo transfer;

			bool IsCompleteForPresentation() const {
				return graphics.index.has_value() && present.index.has_value();
			}
		};

		// ----- physical device selection and enumeration
		void PickPhysicalDevice();
		QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface = nullptr);
//		void EnumeratePhysicalDevices();

		// ----- logical device creation
		void CreateLogicalDevice();

	private:
		Ref<VKInstance> m_Instance{};

		// core vulkan handles
		vk::SurfaceKHR m_Surface{};
		vk::PhysicalDevice m_PhysicalDevice{};
		vk::Device m_LogicalDevice{};
		struct Queues {
			vk::Queue graphics;
			vk::Queue present;
			vk::Queue compute;
			vk::Queue transfer;
		} m_Queues{};
		vk::detail::DispatchLoaderDynamic m_DispatchLoader{};

		// extensions
		struct {
			ExtensionStruct device;
		} m_Extensions{};

		// device selection criteria
		DeviceSelectionCriteria m_SelectionCriteria;

		// selected device queue family indices
		QueueFamilyIndices m_QueueFamilyIndices{};

		// cached physical device list
		std::vector<vk::PhysicalDevice> m_PhysicalDevices;

		// queue family cache: keyed by device+surface handle mix (uint64). populated by FindQueueFamilies
		std::unordered_map<uint64_t, QueueFamilyIndices> m_QueueFamilyCache;
	};


} // namespace Helios::Engine::Renderer::Vulkan
