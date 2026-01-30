//==============================================================================
// Vulkan Device Manager
//
// Encapsulates the selection and management of Vulkan physical and logical
// devices. It is responsible for identifying a suitable GPU, creating a
// logical device for command submission, and managing queue families for
// various operations (graphics, presentation, compute, and transfer).
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - Selects an optimal vk::PhysicalDevice using a scoring-based system.
// - Creates a vk::Device with required extensions and features.
// - Identifies and provides access to graphics, present, compute, and
//   transfer queue families and their corresponding vk::Queues.
// - Manages device-related resources in a RAII-compliant manner.
// 
// Changelog:
// - 2025.01: Added physical device selection persistence
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

namespace Helios::Engine::Renderer::Vulkan {


	// Forward declarations
	class VKInstance;
	class VKSurface;


	// Struct to hold device extensions
	struct DeviceExtensionInfo {
		std::unordered_set<std::string> required = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		std::unordered_set<std::string> optional = { /*VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME*/ };
		std::unordered_set<std::string> supported;
		std::vector<const char*> enabled;
	};


	// Struct to hold the queue family indices we need
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphics;
		std::optional<uint32_t> present;
		std::optional<uint32_t> compute;
		std::optional<uint32_t> transfer;

		bool IsCompleteForPresentation() const { return graphics.has_value() && present.has_value(); }
		bool IsCompleteForCompute() const { return compute.has_value(); }
		bool IsCompleteForTransfer() const { return transfer.has_value(); }
	};


	struct VKQueues {
		vk::Queue graphics;
		vk::Queue present;
		vk::Queue compute;
		vk::Queue transfer;
	};


	class VKDeviceManager
	{
	public:
		VKDeviceManager(const VKInstance& instance, const VKSurface& surface);
		~VKDeviceManager();

		// Prevent copying and moving
		VKDeviceManager(const VKDeviceManager&) = delete;
		VKDeviceManager& operator=(const VKDeviceManager&) = delete;
		VKDeviceManager(VKDeviceManager&&) = delete;
		VKDeviceManager& operator=(VKDeviceManager&&) = delete;

		// --- Accessors ---
		const vk::PhysicalDevice& GetPhysicalDevice() const { return m_physicalDevice; }
		const vk::Device& GetLogicalDevice() const { return m_logicalDevice; }

		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_queueIndices; }
		const VKQueues& GetQueues() const { return m_Queues; }

		const vk::Queue& GetGraphicsQueue() const { return m_Queues.graphics; }
		const vk::Queue& GetPresentQueue() const { return m_Queues.present; }
		const vk::Queue& GetComputeQueue() const { return m_Queues.compute; }
		const vk::Queue& GetTransferQueue() const { return m_Queues.transfer; }
		
		const uint32_t GetGraphicsQueueIndex() const { return m_queueIndices.graphics.value_or(0u); }
		const uint32_t GetPresentQueueIndex() const { return m_queueIndices.present.value_or(0u); }
		const uint32_t GetComputeQueueIndex() const { return m_queueIndices.compute.value_or(0u); }
		const uint32_t GetTransferQueueIndex() const { return m_queueIndices.transfer.value_or(0u); }

	private:
		// --- Initialization Steps ---
		void PickPhysicalDevice(const VKInstance& instance, const VKSurface& surface);
		void CreateLogicalDevice(const VKSurface& surface);

		// --- Helper Functions for Device Selection ---
		vk::PhysicalDevice FindPreferredDevice(const std::vector<vk::PhysicalDevice>& devices, const VKSurface& surface);
		bool IsDeviceSuitable(vk::PhysicalDevice device, const VKSurface& surface);
		int RateDeviceSuitability(vk::PhysicalDevice device, const VKSurface& surface);
		QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, const VKSurface& surface);
		bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);
		// SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, const VKSurface& surface); // TODO: Implement next

	private:
		vk::PhysicalDevice m_physicalDevice = nullptr;
		vk::Device m_logicalDevice = nullptr;

		const VKInstance& m_Instance;

		QueueFamilyIndices m_queueIndices;
		VKQueues m_Queues;

		// Centralized list of device extensions
		DeviceExtensionInfo m_deviceExtensionsInfo;
	};


} // namespace Helios::Engine::Renderer::Vulkan
