//==============================================================================
// Vulkan Instance Wrapper (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VKInstance.h
//==============================================================================
#include "pch.h"
#include "Helios/Platform/Renderer/Vulkan/VKInstance.h"

namespace Helios::Engine::Renderer::Vulkan {


	// Helper function to create a debug messenger.
	// This is a free function because the function pointer must be loaded from the instance.
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}


	// Helper function to destroy a debug messenger.
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}


	VKInstance::VKInstance(const AppSpec& appSpec)
	{
		// Populate required extensions from GLFW
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
			m_instanceExtensionsInfo.required.insert(glfwExtensions[i]);
		}

		// Add optional debug utils extension if validation is enabled
		if (m_enableValidationLayers) {
			m_instanceExtensionsInfo.optional.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		CreateInstance(appSpec);
		SetupDebugMessenger();
	}


	VKInstance::~VKInstance()
	{
		if (m_debugMessenger) {
			DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
			LOG_RENDER_DEBUG("VKInstance: Debug messenger destroyed.");
		}

		if (m_instance) {
			m_instance.destroy();
			LOG_RENDER_DEBUG("VKInstance: Instance destroyed.");
		}
	}


	void VKInstance::CreateInstance(const AppSpec& appSpec)
	{
		if (m_enableValidationLayers && !CheckValidationLayerSupport()) {
			LOG_RENDER_WARN("VKInstance: Validation layers requested, but not available!");
		}

		CheckInstanceExtensionSupport();

		// --- Application Info ---
		vk::ApplicationInfo appInfo = vk::ApplicationInfo()
			.setPApplicationName(appSpec.Name.c_str())
			.setApplicationVersion(appSpec.Version)
			.setPEngineName("Helios Engine")
			.setEngineVersion(HE_VERSION)
			.setApiVersion(VK_API_VERSION_1_3);

		// --- Instance Create Info ---
		vk::InstanceCreateInfo createInfo;
		createInfo.pApplicationInfo = &appInfo;

		// Use the enabled extensions from our struct
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_instanceExtensionsInfo.enabled.size());
		createInfo.ppEnabledExtensionNames = m_instanceExtensionsInfo.enabled.data();

		// Enable validation layers and set up debug messenger for create/destroy
		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (m_enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();

			// Populate the debug messenger create info for instance creation/destruction logging
			debugCreateInfo.flags = vk::DebugUtilsMessengerCreateFlagsEXT();
			debugCreateInfo.messageSeverity =
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
			debugCreateInfo.messageType =
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
				vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
			debugCreateInfo.pfnUserCallback = reinterpret_cast<vk::PFN_DebugUtilsMessengerCallbackEXT>(DebugCallback);
			createInfo.pNext = &debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// --- Create the Instance ---
		m_instance = vk::createInstance(createInfo);
		LOG_RENDER_DEBUG("VKInstance: Instance created.");
	}


	void VKInstance::SetupDebugMessenger()
	{
		// Only create the messenger if the extension was successfully enabled
		bool debugUtilsEnabled = false;
		for(const char* ext : m_instanceExtensionsInfo.enabled) {
			if (strcmp(ext, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
				debugUtilsEnabled = true;
				break;
			}
		}
		if (!debugUtilsEnabled) return;

		vk::DebugUtilsMessengerCreateInfoEXT createInfo;
		createInfo.flags = vk::DebugUtilsMessengerCreateFlagsEXT();
		createInfo.messageSeverity =
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
		createInfo.messageType =
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
		createInfo.pfnUserCallback = reinterpret_cast<vk::PFN_DebugUtilsMessengerCallbackEXT>(DebugCallback);

		// The C++ bindings do not have this function, so we must use the C-style function pointer
		if (CreateDebugUtilsMessengerEXT(m_instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, reinterpret_cast<VkDebugUtilsMessengerEXT*>(&m_debugMessenger)) != VK_SUCCESS) {
			LOG_RENDER_WARN("VKInstance: Failed to set up debug messenger!");
		}
		else {
			LOG_RENDER_DEBUG("VKInstance: Debug messenger created.");
		}
	}


	void VKInstance::CheckInstanceExtensionSupport()
	{
		std::vector<vk::ExtensionProperties> availableExtensions = vk::enumerateInstanceExtensionProperties();
		std::unordered_set<std::string> available;
		for (const auto& ext : availableExtensions) {
			available.insert(ext.extensionName);
		}

		// Check for all required extensions
		for (const auto& requiredExt : m_instanceExtensionsInfo.required) {
			if (available.find(requiredExt) == available.end()) {
				LOG_RENDER_EXCEPT("VKInstance: Required instance extension '{}' not supported.", requiredExt);
			}
			LOG_RENDER_TRACE("VKInstance: Required instance extension '{}' is supported.", requiredExt);
		}

		// If all required extensions are found, build our list of enabled extensions
		m_instanceExtensionsInfo.enabled.clear();
		for (const auto& ext : m_instanceExtensionsInfo.required) {
			m_instanceExtensionsInfo.enabled.push_back(ext.c_str());
		}
		// Add optional extensions if available
		for (const auto& ext : m_instanceExtensionsInfo.optional) {
			if (available.find(ext) != available.end()) {
				m_instanceExtensionsInfo.enabled.push_back(ext.c_str());
				LOG_RENDER_TRACE("VKInstance: Optional instance extension '{}' is supported and enabled.", ext);
			}
		}

		// Store supported extensions for reference
		m_instanceExtensionsInfo.supported = std::move(available);
	}


	bool VKInstance::CheckValidationLayerSupport() const
	{
		auto availableLayers = vk::enumerateInstanceLayerProperties();

		for (const char* layerName : m_validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				LOG_RENDER_WARN("VKInstance: Validation layer NOT found: {}", layerName);
				return false;
			}
			LOG_RENDER_TRACE("VKInstance: Validation layer found: {}", layerName);
		}

		LOG_RENDER_TRACE("VKInstance: All requested validation layers are available.");
		return true;
	}


	VKAPI_ATTR VkBool32 VKAPI_CALL VKInstance::DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		// Format the message
		std::string message = "VKCallback:: " + std::string(pCallbackData->pMessage);

		// Log with appropriate severity
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			LOG_RENDER_ERROR(message);
		}
		else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			LOG_RENDER_WARN(message);
		}
		else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			LOG_RENDER_INFO(message);
		}
		else {
			LOG_RENDER_TRACE(message);
		}

		return VK_FALSE;
	}


} // namespace Helios::Engine::Renderer::Vulkan
