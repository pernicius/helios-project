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
		CreateInstance(appSpec);
		SetupDebugMessenger();
	}


	VKInstance::~VKInstance()
	{
		if (m_enableValidationLayers) {
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

		// --- Application Info ---
		vk::ApplicationInfo appInfo(
			appSpec.Name.c_str(),
			appSpec.Version,
			"Helios Engine",
			HE_VERSION,
			VK_API_VERSION_1_3
		);

		// --- Instance Create Info ---
		vk::InstanceCreateInfo createInfo;
		createInfo.pApplicationInfo = &appInfo;

		// Get extensions required by GLFW and our debug messenger
		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		// Enable validation layers and set up debug messenger for create/destroy
		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (m_enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();

			// Populate the debug messenger create info for instance creation/destruction logging
			debugCreateInfo.flags = vk::DebugUtilsMessengerCreateFlagsEXT();
			debugCreateInfo.messageSeverity =
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
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
		if (!m_enableValidationLayers) return;

		vk::DebugUtilsMessengerCreateInfoEXT createInfo;
		createInfo.flags = vk::DebugUtilsMessengerCreateFlagsEXT();
		createInfo.messageSeverity =
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
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


	std::vector<const char*> VKInstance::GetRequiredExtensions() const
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
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
				LOG_RENDER_WARN("VKInstance: Validation layer not found: {}", layerName);
				return false;
			}
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
