//==============================================================================
// Vulkan Device Manager (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VKDeviceManager.h
//==============================================================================
#include "pch.h"
#include "Helios/Platform/Renderer/Vulkan/VKDeviceManager.h"

#include "Helios/Platform/Renderer/Vulkan/VKInstance.h"
#include "Helios/Platform/Renderer/Vulkan/VKSurface.h"

#include "Helios/Engine/Core/Config.h"

namespace Helios::Engine::Renderer::Vulkan {


	VKDeviceManager::VKDeviceManager(const VKInstance& instance, const VKSurface& surface)
		: m_Instance(instance)
	{
		PickPhysicalDevice(instance, surface);
		CreateLogicalDevice(surface);
	}


	VKDeviceManager::~VKDeviceManager()
	{
		if (m_logicalDevice) {
			m_logicalDevice.destroy();
			LOG_RENDER_DEBUG("VKDevMgr: Logical device destroyed.");
		}
	}


	void VKDeviceManager::PickPhysicalDevice(const VKInstance& instance, const VKSurface& surface)
	{
		std::vector<vk::PhysicalDevice> devices = instance.Get().enumeratePhysicalDevices();
		if (devices.empty()) {
			LOG_RENDER_EXCEPT("VKDevMgr: Failed to find GPUs with Vulkan support!");
		}

		LOG_RENDER_INFO("VKDevMgr: Found {} device(s).", devices.size());

		// First, try to find the preferred device from config
		m_physicalDevice = FindPreferredDevice(devices, surface);

		if (m_physicalDevice) {
			vk::PhysicalDeviceProperties properties = m_physicalDevice.getProperties();
			LOG_RENDER_INFO("VKDevMgr: Selected preferred physical device: {}", properties.deviceName.data());
		}
		else {
			LOG_RENDER_INFO("VKDevMgr: No preferred device found or suitable. Selecting best alternative.");
			// Use a map to store candidates and their scores
			std::multimap<int, vk::PhysicalDevice> candidates;

			for (const auto& device : devices) {
				int score = RateDeviceSuitability(device, surface);
				candidates.insert(std::make_pair(score, device));
				vk::PhysicalDeviceProperties properties = device.getProperties();
				LOG_RENDER_DEBUG("VKDevMgr: Found physical device: {} (Score: {})", properties.deviceName.data(), score);
			}

			// Check if the best candidate is suitable
			if (candidates.rbegin()->first > 0) {
				m_physicalDevice = candidates.rbegin()->second;
				vk::PhysicalDeviceProperties properties = m_physicalDevice.getProperties();
				LOG_RENDER_INFO("VKDevMgr: Selected physical device: {} (Score: {})", properties.deviceName.data(), candidates.rbegin()->first);
			}
			else {
				LOG_RENDER_EXCEPT("VKDevMgr: Failed to find a suitable GPU!");
			}
		}

		if (!m_physicalDevice) {
			LOG_RENDER_EXCEPT("VKDevMgr: Failed to find a suitable GPU!");
		}

		// Save the selected device's info for next time
		vk::PhysicalDeviceProperties properties = m_physicalDevice.getProperties();
		ConfigManager::GetInstance().Set<std::string>("renderer_vulkan", "PhysicalDevice", "PreferredVendorID", std::to_string(properties.vendorID));
		ConfigManager::GetInstance().Set<std::string>("renderer_vulkan", "PhysicalDevice", "PreferredDeviceID", std::to_string(properties.deviceID));
	}


	void VKDeviceManager::CreateLogicalDevice(const VKSurface& surface)
	{
		m_queueIndices = FindQueueFamilies(m_physicalDevice, surface);

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			m_queueIndices.graphics.value(),
			m_queueIndices.present.value()
		};
		if (m_queueIndices.compute.has_value()) { uniqueQueueFamilies.insert(m_queueIndices.compute.value()); }
		if (m_queueIndices.transfer.has_value()) { uniqueQueueFamilies.insert(m_queueIndices.transfer.value()); }

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			vk::DeviceQueueCreateInfo queueCreateInfo({}, queueFamily, 1, &queuePriority);
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures deviceFeatures{}; // No special features needed for now

		// Use the list of enabled extensions we gathered during device suitability checks
		vk::DeviceCreateInfo createInfo;
		createInfo.setQueueCreateInfos(queueCreateInfos);
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.setEnabledExtensionCount(static_cast<uint32_t>(m_deviceExtensionsInfo.enabled.size()));
		createInfo.ppEnabledExtensionNames = m_deviceExtensionsInfo.enabled.data();

		// For compatibility with older Vulkan implementations
		if (m_Instance.AreValidationLayersEnabled()) {
			const auto& validationLayers = m_Instance.GetValidationLayers();
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}

		m_logicalDevice = m_physicalDevice.createDevice(createInfo);
		LOG_RENDER_DEBUG("VKDevMgr: Logical device created.");

		// Retrieve queue handles
		m_Queues.graphics = m_logicalDevice.getQueue(m_queueIndices.graphics.value(), 0);
		m_Queues.present = m_logicalDevice.getQueue(m_queueIndices.present.value(), 0);
		if (m_queueIndices.compute.has_value()) {
			m_Queues.compute = m_logicalDevice.getQueue(m_queueIndices.compute.value(), 0);
		}
		if (m_queueIndices.transfer.has_value()) {
			m_Queues.transfer = m_logicalDevice.getQueue(m_queueIndices.transfer.value(), 0);
		}
		LOG_RENDER_DEBUG("VKDevMgr: Queues created (Graphics: {}, Present: {}, Compute: {}, Transfer: {}).",
			m_queueIndices.graphics.value(),
			m_queueIndices.present.value(),
			m_queueIndices.compute.has_value() ? std::to_string(m_queueIndices.compute.value()) : "N/A",
			m_queueIndices.transfer.has_value() ? std::to_string(m_queueIndices.transfer.value()) : "N/A");
	}


	vk::PhysicalDevice VKDeviceManager::FindPreferredDevice(const std::vector<vk::PhysicalDevice>& devices, const VKSurface& surface)
	{
		std::string preferredVendorID_str = ConfigManager::GetInstance().Get<std::string>("renderer_vulkan", "PhysicalDevice", "PreferredVendorID");
		std::string preferredDeviceID_str = ConfigManager::GetInstance().Get<std::string>("renderer_vulkan", "PhysicalDevice", "PreferredDeviceID");

		if (preferredVendorID_str.empty() || preferredDeviceID_str.empty()) {
			return nullptr; // No preferred device configured
		}

		uint32_t preferredVendorID = std::stoul(preferredVendorID_str);
		uint32_t preferredDeviceID = std::stoul(preferredDeviceID_str);

		for (const auto& device : devices) {
			vk::PhysicalDeviceProperties properties = device.getProperties();
			if (properties.vendorID == preferredVendorID && properties.deviceID == preferredDeviceID) {
				if (IsDeviceSuitable(device, surface)) {
					LOG_RENDER_DEBUG("VKDevMgr: Found matching preferred device: {}", properties.deviceName.data());
					return device;
				}
				else {
					LOG_RENDER_WARN("VKDevMgr: Found preferred device '{}', but it is no longer suitable.", properties.deviceName.data());
				}
			}
		}

		return nullptr;
	}


	bool VKDeviceManager::IsDeviceSuitable(vk::PhysicalDevice device, const VKSurface& surface)
	{
		QueueFamilyIndices indices = FindQueueFamilies(device, surface);
		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			// TODO: Implement QuerySwapChainSupport to get formats and present modes
			// SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
			// swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			swapChainAdequate = true; // Placeholder for now
		}

		return indices.IsCompleteForPresentation() && extensionsSupported && swapChainAdequate;
	}


	int VKDeviceManager::RateDeviceSuitability(vk::PhysicalDevice device, const VKSurface& surface)
	{
		if (!IsDeviceSuitable(device, surface))
			return 0;

		int score = 0;
		vk::PhysicalDeviceProperties properties = device.getProperties();
		vk::PhysicalDeviceFeatures features = device.getFeatures();

		// Prefer discrete GPUs
		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			score += 1000;
		if (properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
			score += 500;

		// Prefer devices with separate dedicated compute/transfer/present families
		QueueFamilyIndices indices = FindQueueFamilies(device, surface);
		if (indices.compute.has_value() && indices.compute.value() != indices.graphics.value())
			score += 50; // Has a dedicated compute queue
		if (indices.transfer.has_value() && indices.transfer.value() != indices.graphics.value() && indices.transfer.value() != indices.compute.value())
			score += 30; // Has a dedicated transfer queue
		if (indices.present.has_value() && indices.present.value() != indices.graphics.value())
			score += 10; // Has a dedicated present queue
		
		// Maximum texture size adds to score
		score += properties.limits.maxImageDimension2D / 1024;

		return score;
	}


	QueueFamilyIndices VKDeviceManager::FindQueueFamilies(vk::PhysicalDevice device, const VKSurface& surface)
	{
		QueueFamilyIndices indices;
		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

		// --- Heuristic Queue Selection (Multi-Pass) ---
		// Goal: Find the most specialized queue for each purpose.
		// A lower score is better.
		// Score 1: Dedicated (e.g., only compute).
		// Score 2: Shared (e.g., compute + graphics).
		auto scoreQueue = [](const vk::QueueFamilyProperties& props, vk::QueueFlagBits target) {
			if (!(props.queueFlags & target)) return 100; // Not a candidate

			int score = 0;
			if (props.queueFlags & vk::QueueFlagBits::eGraphics) score++;
			if (props.queueFlags & vk::QueueFlagBits::eCompute) score++;
			if (props.queueFlags & vk::QueueFlagBits::eTransfer) score++;
			return score;
			};

		// Candidates: map from score to queue family index
		std::multimap<int, uint32_t> graphicsCandidates, computeCandidates, transferCandidates, presentCandidates;

		for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
			const auto& family = queueFamilies[i];
			graphicsCandidates.insert({ scoreQueue(family, vk::QueueFlagBits::eGraphics), i });
			computeCandidates.insert({ scoreQueue(family, vk::QueueFlagBits::eCompute), i });
			transferCandidates.insert({ scoreQueue(family, vk::QueueFlagBits::eTransfer), i });

			if (device.getSurfaceSupportKHR(i, surface.Get())) {
				// For present, prefer a non-graphics queue
				int presentScore = (family.queueFlags & vk::QueueFlagBits::eGraphics) ? 1 : 0;
				presentCandidates.insert({ presentScore, i });
			}
		}

		// --- Assign best candidates, avoiding reuse if possible ---
		std::set<uint32_t> assignedQueues;

		// Must have graphics and present
		indices.graphics = graphicsCandidates.begin()->second;
		assignedQueues.insert(indices.graphics.value());
		indices.present = presentCandidates.begin()->second;
		// No need to add present to assignedQueues, as it can share with graphics

		// Find best compute queue that is not already assigned
		for (auto const& [score, index] : computeCandidates) {
			if (assignedQueues.find(index) == assignedQueues.end()) {
				indices.compute = index;
				break;
			}
		}
		if (indices.compute.has_value()) {
			assignedQueues.insert(indices.compute.value());
		}

		// Find best transfer queue that is not already assigned
		for (auto const& [score, index] : transferCandidates) {
			if (assignedQueues.find(index) == assignedQueues.end()) {
				indices.transfer = index;
				break;
			}
		}

		// --- Fallbacks ---
		// If no dedicated queues were found, fall back to less specialized ones.
		if (!indices.compute.has_value()) {
			indices.compute = computeCandidates.begin()->second; // Best available compute, even if shared
		}
		if (!indices.transfer.has_value()) {
			indices.transfer = transferCandidates.begin()->second; // Best available transfer, even if shared
		}

		return indices;
	}


	bool VKDeviceManager::CheckDeviceExtensionSupport(vk::PhysicalDevice device)
	{
		std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();
		std::unordered_set<std::string> available;
		for (const auto& ext : availableExtensions) {
			available.insert(ext.extensionName);
		}

		// Check for all required extensions
		for (const auto& requiredExt : m_deviceExtensionsInfo.required) {
			if (available.find(requiredExt) == available.end()) {
				LOG_RENDER_WARN("VKDevMgr: Required device extension '{}' not supported.", requiredExt);
				return false;
			}
		}

		// If all required extensions are found, build our list of enabled extensions
		m_deviceExtensionsInfo.enabled.clear();
		for (const auto& ext : m_deviceExtensionsInfo.required) {
			m_deviceExtensionsInfo.enabled.push_back(ext.c_str());
		}
		// Add optional extensions if available
		for (const auto& ext : m_deviceExtensionsInfo.optional) {
			if (available.find(ext) != available.end()) {
				m_deviceExtensionsInfo.enabled.push_back(ext.c_str());
			}
		}

		// Store supported extensions for reference
		m_deviceExtensionsInfo.supported = std::move(available);

		return true;
	}

} // namespace Helios::Engine::Renderer::Vulkan
