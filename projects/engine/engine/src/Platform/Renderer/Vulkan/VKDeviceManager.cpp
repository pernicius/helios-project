#include "pch.h"
#include "Platform/Renderer/Vulkan/VKDeviceManager.h"

#include "Helios/Engine/Spec/SpecApp.h"

namespace Helios::Engine::Renderer {


	// Debug callback (C-style signature required by Vulkan)
	static VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (pCallbackData && pCallbackData->pMessage) {
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
		}

		// return VK_TRUE to indicate that the call should be aborted (rarely desired)
		return VK_FALSE;
	}


	VKDeviceManager::VKDeviceManager()
	{
	}


	VKDeviceManager::~VKDeviceManager()
	{
		if (m_Instance) {
			DestroyInstance();
		}
	}


	void VKDeviceManager::Init()
	{
		CreateInstance();
	}


	void VKDeviceManager::EnumerateInstanceExtensionNames()
	{
		auto props = vk::enumerateInstanceExtensionProperties();
		m_ExtensionsSupported.clear();
		m_ExtensionsSupported.reserve(props.size());
		for (const auto& p : props) {
			m_ExtensionsSupported.emplace(std::string(p.extensionName.data()));
		}
	}


	void VKDeviceManager::EnumerateInstanceLayerNames()
	{
		auto props = vk::enumerateInstanceLayerProperties();
		m_LayersSupported.clear();
		m_LayersSupported.reserve(props.size());
		for (const auto& p : props) {
			m_LayersSupported.emplace(std::string(p.layerName.data()));
		}
	}


	// Query the Vulkan loader for the highest instance API version supported.
	static uint32_t QuerySupportedInstanceVersion()
	{
		uint32_t version = VK_API_VERSION_1_0;
		// enumerateInstanceVersion is available on recent loaders; if it throws, fallback to 1.0
		try {
			version = vk::enumerateInstanceVersion();
		}
		catch (const vk::SystemError&) {
			// keep version = 1.0
		}
		return version;
	}


	void VKDeviceManager::CreateInstance()
	{
		LOG_RENDER_DEBUG("VKDeviceManager: Creating instance.");

		// Check vulkan loader/runtime version and pick an API version we can use.
		const uint32_t requestedApiVersion = VK_API_VERSION_1_4;
		const uint32_t supportedApiVersion = QuerySupportedInstanceVersion();
		const uint32_t chosenApiVersion = std::min(requestedApiVersion, supportedApiVersion);

		if (chosenApiVersion < VK_API_VERSION_1_0) {
			LOG_RENDER_FATAL("Vulkan loader reports no usable instance API version.");
			throw std::runtime_error("Unsupported Vulkan loader version.");
		}

		// setup application info
		vk::ApplicationInfo appInfo{};
		appInfo.pApplicationName = Spec::App::Name.c_str();
		appInfo.applicationVersion = Spec::App::Version;
		appInfo.pEngineName = "Helios";
		appInfo.engineVersion = HE_VERSION;
		appInfo.apiVersion = chosenApiVersion;

		// cache supported instance extensions early so we only request those that exist
		EnumerateInstanceExtensionNames();

		// build requested extensions as owned strings to avoid pointer lifetime issues
		std::vector<std::string> requestedExtensions;
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		if (!glfwExtensions || glfwExtensionCount == 0) {
			LOG_RENDER_FATAL("GLFW did not return any required instance extensions.");
			throw std::runtime_error("GLFW required instance extensions missing.");
		}

		requestedExtensions.reserve(glfwExtensionCount + 1);
		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
			requestedExtensions.emplace_back(glfwExtensions[i]);

#		ifdef BUILD_DEBUG
			// Only request debug utils if supported by the loader
			if (IsExtensionSupported("VK_EXT_debug_utils")) {
				requestedExtensions.emplace_back("VK_EXT_debug_utils");
			}
			else {
				LOG_RENDER_WARN("VKDeviceManager: Debug utils extension not available; continuing without it.");
			}
#		endif

		// check extension support
		bool allExtensionsSupported = true;
		for (const auto& ext : requestedExtensions) {
			if (IsExtensionSupported(ext.c_str())) {
				LOG_RENDER_DEBUG("VKDeviceManager: Required extension [ OK ]: {}", ext);
			}
			else {
				LOG_RENDER_ERROR("VKDeviceManager: Required extension [FAIL]: {}", ext);
				allExtensionsSupported = false;
			}
		}
		if (!allExtensionsSupported) {
			LOG_RENDER_FATAL("Required vulkan instance extensions not supported!");
			throw std::runtime_error("Required vulkan instance extensions not supported!");
		}
		// store enabled names (owned)
		m_ExtensionsEnabled = requestedExtensions;

		// build requested layers as owned strings
		std::vector<std::string> requestedLayers;
#		ifdef BUILD_DEBUG
			requestedLayers.emplace_back("VK_LAYER_KHRONOS_validation");
#		endif

		// check layer support (cache supported first)
		EnumerateInstanceLayerNames();
		bool allLayersSupported = true;
		for (const auto& layer : requestedLayers) {
			if (IsLayerSupported(layer.c_str())) {
				LOG_RENDER_DEBUG("VKDeviceManager: Required layer [ OK ]: {}", layer);
			}
			else {
				LOG_RENDER_ERROR("VKDeviceManager: Required layer [FAIL]: {}", layer);
				allLayersSupported = false;
			}
		}
		if (!allLayersSupported) {
			LOG_RENDER_FATAL("Required vulkan instance layers not supported!");
			throw std::runtime_error("Required vulkan instance layers not supported!");
		}
		// store enabled layers (owned)
		m_LayersEnabled = requestedLayers;

		// prepare raw pointers for vk create info (guaranteed safe because strings are owned by vectors above)
		std::vector<const char*> extCStrs;
		extCStrs.reserve(m_ExtensionsEnabled.size());
		for (const auto& s : m_ExtensionsEnabled) extCStrs.push_back(s.c_str());

		std::vector<const char*> layerCStrs;
		layerCStrs.reserve(m_LayersEnabled.size());
		for (const auto& s : m_LayersEnabled) layerCStrs.push_back(s.c_str());

		// setup instance create info
		vk::InstanceCreateInfo createInfo{};
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = static_cast<uint32_t>(layerCStrs.size());
		createInfo.ppEnabledLayerNames = layerCStrs.empty() ? nullptr : layerCStrs.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extCStrs.size());
		createInfo.ppEnabledExtensionNames = extCStrs.empty() ? nullptr : extCStrs.data();

		// finally try to create the instance
		try {
			m_Instance = vk::createInstance(createInfo);
		}
		catch (const vk::SystemError& err) {
			LOG_RENDER_FATAL("Failed to create Vulkan instance: {}", err.what());
			throw std::runtime_error("Failed to create Vulkan instance!");
		}

		// build dispatch loader using vkGetInstanceProcAddr so extension entry points are resolved
		m_DispatchLoader = vk::detail::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr);

		// Setup debug messenger (after instance creation) in debug builds
		SetupDebugMessenger();
	}


	void VKDeviceManager::DestroyInstance()
	{
		DestroyDebugMessenger();

		if (m_Instance) {
			m_Instance.destroy();
			// reset to default-constructed handle to make destructor idempotent
			m_Instance = vk::Instance();
			LOG_RENDER_DEBUG("VKDeviceManager: Destroyed instance.");
		}
	}


	void VKDeviceManager::SetupDebugMessenger()
	{
#		ifdef BUILD_DEBUG
			LOG_RENDER_DEBUG("VKDeviceManager: Creating DebugCallback.");

			if (!m_Instance || !IsExtensionEnabled("VK_EXT_debug_utils"))
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
				LOG_RENDER_DEBUG("VKDeviceManager: DebugCallback created.");
			}
			catch (const vk::SystemError& err) {
				LOG_RENDER_ERROR("VKDeviceManager: Failed to create DebugCallback: {}", err.what());
			}
#		endif
	}


	void VKDeviceManager::DestroyDebugMessenger()
	{
#		ifdef BUILD_DEBUG
			if (m_Instance && m_DebugMessenger) {
				try {
					m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DispatchLoader);
					LOG_RENDER_DEBUG("VKDeviceManager: Destroyed DebugCallback.");
				}
				catch (const vk::SystemError& err) {
					LOG_RENDER_ERROR("VKDeviceManager: Failed to destroy debug messenger: {}", err.what());
				}
				m_DebugMessenger = nullptr;
			}
#		endif
	}


} // namespace Helios::Engine::Renderer
