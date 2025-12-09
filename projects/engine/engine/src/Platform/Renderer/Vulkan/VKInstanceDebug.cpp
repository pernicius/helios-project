#include "pch.h"
#include "Platform/Renderer/Vulkan/VKInstance.h"

namespace Helios::Engine::Renderer::Vulkan {


	// Debug callback (C-style signature required by Vulkan)
	static VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugUtilsMessengerCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (pCallbackData && pCallbackData->pMessage) {

			//============================
			// TEMPORARY !!!
			// 
			// vkCreateDevice triggers many "Loader Message" validation messages that are not useful
			// 
			// Filter messages whose pMessageIdName is exactly "Loader Message"
			if (pCallbackData->pMessageIdName && std::strcmp(pCallbackData->pMessageIdName, "Loader Message") == 0) { return VK_FALSE; }
			//============================

			switch (messageSeverity)
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				LOG_RENDER_TRACE("VKValidationCallback({}): {}", pCallbackData->pMessageIdName, pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				LOG_RENDER_INFO("VKValidationCallback({}): {}", pCallbackData->pMessageIdName, pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				LOG_RENDER_WARN("VKValidationCallback({}): {}", pCallbackData->pMessageIdName, pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				LOG_RENDER_ERROR("VKValidationCallback({}): {}", pCallbackData->pMessageIdName, pCallbackData->pMessage);
				break;
			default:
				LOG_RENDER_ERROR("VKValidationCallback({}): {}", pCallbackData->pMessageIdName, pCallbackData->pMessage);
				break;
			}
		}

		// return VK_TRUE to indicate that the call should be aborted (rarely desired)
		return VK_FALSE;
	}


	void VKInstance::InstallDebugUtilsMessenger()
	{
#		ifdef BUILD_DEBUG
			LOG_RENDER_DEBUG("VKInstance: Creating DebugUtilsMessengerCallback.");

			if (!m_Extensions.instance.enabled.contains("VK_EXT_debug_utils"))
				return;

			vk::DebugUtilsMessageSeverityFlagsEXT severity =
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

			vk::DebugUtilsMessageTypeFlagsEXT types =
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
				vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

			vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
			createInfo.messageSeverity = severity;
			createInfo.messageType = types;
			createInfo.pfnUserCallback = reinterpret_cast<vk::PFN_DebugUtilsMessengerCallbackEXT>(VKDebugUtilsMessengerCallback);
			createInfo.pUserData = nullptr;

			try {
				m_DebugUtilsMessenger = m_Instance.createDebugUtilsMessengerEXT(createInfo, nullptr, m_DispatchLoader);
			}
			catch (const vk::SystemError& err) {
				LOG_RENDER_ERROR("VKInstance: Failed to create DebugUtilsMessengerCallback: {}", err.what());
			}
#       else
			LOG_RENDER_DEBUG("VKInstance: Creating DebugUtilsMessengerCallback... (skipped)");
#		endif
	}


	void VKInstance::RemoveDebugUtilsMessenger()
	{
#		ifdef BUILD_DEBUG
			if (m_DebugUtilsMessenger) {
				try {
					m_Instance.destroyDebugUtilsMessengerEXT(m_DebugUtilsMessenger, nullptr, m_DispatchLoader);
					LOG_RENDER_DEBUG("VKInstance: Destroyed DebugUtilsMessengerCallback.");
				}
				catch (const vk::SystemError& err) {
					LOG_RENDER_ERROR("VKInstance: Failed to destroy DebugUtilsMessengerCallback: {}", err.what());
				}
				m_DebugUtilsMessenger = vk::DebugUtilsMessengerEXT{};
			}
#		endif
	}


} // namespace Helios::Engine::Renderer::Vulkan
