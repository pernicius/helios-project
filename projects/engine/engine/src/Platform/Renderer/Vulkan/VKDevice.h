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
		VKDevice(Ref<VKInstance> instance, ExtensionStruct& extensions);
		~VKDevice();

	public:
		const vk::Device& Get() const { return m_LogicalDevice; }
		const vk::detail::DispatchLoaderDynamic& GetDispatchLoader() const { return m_DispatchLoader; }
		
		const ExtensionStruct& GetExtensions() const { return m_Extensions.device; }

	private:
		// Simple, configurable criteria used when choosing a physical device.
		struct DeviceSelectionCriteria
		{
			// Require swapchain support (useful when creating a present-capable device).
			// Default true for typical windowed applications.
			bool requireSwapchain = true;

			// Prefer discrete GPUs when scoring.
			bool preferDiscrete = true;

			// Weights used while scoring devices (tweakable).
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

		// ----- Physical device selection and enumeration
		void PickPhysicalDevice();
		QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface = nullptr);
//		void EnumeratePhysicalDevices();

		// ----- Logical device creation
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
