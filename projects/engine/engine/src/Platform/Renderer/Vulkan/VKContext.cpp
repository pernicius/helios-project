#include "pch.h"
#include "Platform/Renderer/Vulkan/VKContext.h"

//#include "Helios/Engine/Renderer/Format.h"
#include "Helios/Engine/Spec/SpecApp.h"
//#include "Helios/Engine/Spec/SpecWindow.h"
//#include "Helios/Engine/Spec/SpecDevice.h"

namespace Helios::Engine::Renderer {


	// Debug callback (C-style signature required by Vulkan)
	static VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			LOG_RENDER_TRACE("VKValidationLayer: {}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			LOG_RENDER_INFO("VKValidationLayer: {}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			LOG_RENDER_WARN("VKValidationLayer: {}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			LOG_RENDER_ERROR("VKValidationLayer: {}", pCallbackData->pMessage);
			break;
		default:
			LOG_RENDER_ERROR("VKValidationLayer: {}", pCallbackData->pMessage);
			break;
		}

		// return VK_TRUE to indicate that the call should be aborted (rarely desired)
		return VK_FALSE;
	}


	VKContext::VKContext()
	{
	}


	VKContext::~VKContext()
	{
		if (m_Instance) {
			DestroyInstance();
		}
	}


	void VKContext::Init()
	{
		CreateInstance();
	}


	bool VKContext::CheckExtensionsSupport(const std::vector<const char*>& requiredExtensions)
	{
		// get available extensions
		std::vector<vk::ExtensionProperties> availableExtensions = vk::enumerateInstanceExtensionProperties();

		// check for each required extension one by one
		bool found;
		for (const char* extension : requiredExtensions) {
			found = false;
			for (const vk::ExtensionProperties& availableExtension : availableExtensions) {
				if (strcmp(extension, availableExtension.extensionName) == 0) {
					found = true;
					LOG_RENDER_DEBUG("VKContext: Required extension found: {}", extension);
					break;
				}
			}
			if (!found) {
				LOG_RENDER_ERROR("VKContext: Required extension NOT found: {}", extension);
				return false;
			}
		}

		return true;
	}


	bool VKContext::CheckLayerSupport(const std::vector<const char*>& requiredLayers)
	{
		// get available layers
		std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

		// check for each required layer one by one
		bool found;
		for (const char* layer : requiredLayers) {
			found = false;
			for (const vk::LayerProperties& availableLayer : availableLayers) {
				if (strcmp(layer, availableLayer.layerName) == 0) {
					found = true;
					LOG_RENDER_DEBUG("VKContext: Required layer found: {}", layer);
					break;
				}
			}
			if (!found) {
				LOG_RENDER_ERROR("VKContext: Required layer NOT found: {}", layer);
				return false;
			}
		}

		return true;
	}


	void VKContext::CreateInstance()
	{
		LOG_RENDER_DEBUG("VKContext: Creating instance.");

		//----------------------------
		// TODO: check vulkan version.
		//----------------------------

		// setup application info
		vk::ApplicationInfo appInfo{};
		appInfo.pApplicationName = Spec::App::Name.c_str();
		appInfo.applicationVersion = Spec::App::Version;
		appInfo.pEngineName = "Helios";
		appInfo.engineVersion = HE_VERSION;
//		appInfo.apiVersion = VK_API_VERSION_1_0;
//		appInfo.apiVersion = VK_API_VERSION_1_1;
//		appInfo.apiVersion = VK_API_VERSION_1_2;
//		appInfo.apiVersion = VK_API_VERSION_1_3;
		appInfo.apiVersion = VK_API_VERSION_1_4;

		// setup required extensions from GLFW
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#		ifdef BUILD_DEBUG
			extensions.push_back("VK_EXT_debug_utils");
#		endif

		if (!CheckExtensionsSupport(extensions)) {
			LOG_RENDER_FATAL("Vulkan instance extensions not supported!");
			throw std::runtime_error("Vulkan instance extensions not supported!");
		}

		// setup required layers
		std::vector<const char*> layers;

#		ifdef BUILD_DEBUG
			layers.push_back("VK_LAYER_KHRONOS_validation");
#		endif
		
		if (!CheckLayerSupport(layers)) {
			LOG_RENDER_FATAL("Vulkan instance layers not supported!");
			throw std::runtime_error("Vulkan instance layers not supported!");
		}

		// setup instance create info
		vk::InstanceCreateInfo createInfo{};
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		// finally try to create the instance
		try {
			m_Instance = vk::createInstance(createInfo);
		}
		catch (const vk::SystemError& err) {
			LOG_RENDER_FATAL("Failed to create Vulkan instance: {}", err.what());
			throw std::runtime_error("Failed to create Vulkan instance!");
		}

		auto xyz = vk::detail::getDispatchLoaderStatic();

		// build dispatch loader using vkGetInstanceProcAddr so extension entry points are resolved
		m_DispatchLoader = vk::detail::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr);

		// Setup debug messenger (after instance creation) in debug builds
		SetupDebugMessenger();
	}


	void VKContext::DestroyInstance()
	{

		// destroy debug messenger before destroying the instance
		DestroyDebugMessenger();

		if (m_Instance) {
			m_Instance.destroy();
			LOG_RENDER_DEBUG("VKContext: Destroyed instance.");
		}
	}


	void VKContext::SetupDebugMessenger()
	{
#		ifdef BUILD_DEBUG
			// Make sure extension is available and instance is valid
			if (!m_Instance)
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
			createInfo.pfnUserCallback = reinterpret_cast<vk::PFN_DebugUtilsMessengerCallbackEXT>(VKDebugCallback);
			createInfo.pUserData = nullptr;

			try {
				m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(createInfo, nullptr, m_DispatchLoader);
				LOG_RENDER_DEBUG("VKContext: Debug messenger created.");
			}
			catch (const vk::SystemError& err) {
				LOG_RENDER_ERROR("VKContext: Failed to create debug messenger: {}", err.what());
			}
#		endif
	}


	void VKContext::DestroyDebugMessenger()
	{
#		ifdef BUILD_DEBUG
			if (m_Instance && m_DebugMessenger) {
				try {
					m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DispatchLoader);
					LOG_RENDER_DEBUG("VKContext: Debug messenger destroyed.");
				}
				catch (const vk::SystemError& err) {
					LOG_RENDER_ERROR("VKContext: Failed to destroy debug messenger: {}", err.what());
				}
				m_DebugMessenger = nullptr;
			}
#		endif
	}


} // namespace Helios::Engine::Renderer
