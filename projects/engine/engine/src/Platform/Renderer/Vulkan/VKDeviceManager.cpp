#include "pch.h"
#include "Platform/Renderer/Vulkan/VKDeviceManager.h"

#include "Helios/Engine/Spec/SpecApp.h"

#include <unordered_set>

//#undef LOG_LEVEL
//#define LOG_LEVEL LOG_LEVEL_DEBUG

namespace Helios::Engine::Renderer {


	// Debug callback (C-style signature required by Vulkan)
	static VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugUtilsMessengerCallback(
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
		if (m_Device) {
			DestroyLogicalDevice();
		}
		if (m_Instance) {
			DestroyInstance();
		}
	}


	void VKDeviceManager::Init()
	{
		CreateInstance();

		EnumeratePhysicalDevices();
		PickPhysicalDevice();
		CreateLogicalDevice();
	}


	//--------------------------------
	// vk::Instance related functions
	//--------------------------------


	void VKDeviceManager::EnumerateInstanceExtensionNames()
	{
		auto props = vk::enumerateInstanceExtensionProperties();
		m_ExtensionsSupported.instance.clear();
		m_ExtensionsSupported.instance.reserve(props.size());
		for (const auto& p : props) {
			m_ExtensionsSupported.instance.emplace_back(std::string(p.extensionName.data()));
			LOG_RENDER_TRACE("VKDeviceManager: Found instance extension: {}", p.extensionName.data());
		}
	}


	void VKDeviceManager::EnumerateInstanceLayerNames()
	{
		auto props = vk::enumerateInstanceLayerProperties();
		m_ExtensionsSupported.layer.clear();
		m_ExtensionsSupported.layer.reserve(props.size());
		for (const auto& p : props) {
			m_ExtensionsSupported.layer.emplace_back(std::string(p.layerName.data()));
			LOG_RENDER_TRACE("VKDeviceManager: Found instance layer: {}", p.layerName.data());
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
			if (std::find(m_ExtensionsSupported.instance.begin(), m_ExtensionsSupported.instance.end(), "VK_EXT_debug_utils")
				!= m_ExtensionsSupported.instance.end()) {
				requestedExtensions.emplace_back("VK_EXT_debug_utils");
			}
			else {
				LOG_RENDER_WARN("VKDeviceManager: Debug utils extension not available; continuing without it.");
			}
#		endif

		// check extension support
		bool allExtensionsSupported = true;
		for (const auto& ext : requestedExtensions) {
			if (std::find(m_ExtensionsSupported.instance.begin(), m_ExtensionsSupported.instance.end(), ext)
				!= m_ExtensionsSupported.instance.end()) {
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
		m_ExtensionsEnabled.instance = requestedExtensions;

		// build requested layers as owned strings
		std::vector<std::string> requestedLayers;
#		ifdef BUILD_DEBUG
			requestedLayers.emplace_back("VK_LAYER_KHRONOS_validation");
#		endif

		// check layer support (cache supported first)
		EnumerateInstanceLayerNames();
		bool allLayersSupported = true;
		for (const auto& layer : requestedLayers) {
			if (std::find(m_ExtensionsSupported.layer.begin(), m_ExtensionsSupported.layer.end(), layer)
				!= m_ExtensionsSupported.layer.end()) {
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
		m_ExtensionsEnabled.layer = requestedLayers;

		// prepare raw pointers for vk create info (guaranteed safe because strings are owned by vectors above)
		std::vector<const char*> extCStrs;
		extCStrs.reserve(m_ExtensionsEnabled.instance.size());
		for (const auto& s : m_ExtensionsEnabled.instance) extCStrs.push_back(s.c_str());

		std::vector<const char*> layerCStrs;
		layerCStrs.reserve(m_ExtensionsEnabled.layer.size());
		for (const auto& s : m_ExtensionsEnabled.layer) layerCStrs.push_back(s.c_str());

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
		m_InstanceDispatch = vk::detail::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr);

		// Setup debug messenger (after instance creation) in debug builds
		SetupDebugUtilsMessenger();
	}


	void VKDeviceManager::DestroyInstance()
	{
		DestroyDebugUtilsMessenger();

		if (m_Instance) {
			m_Instance.destroy();
			// reset to default-constructed handle to make destructor idempotent
			m_Instance = vk::Instance();
			LOG_RENDER_DEBUG("VKDeviceManager: Destroyed instance.");
		}
	}


	void VKDeviceManager::SetupDebugUtilsMessenger()
	{
#		ifdef BUILD_DEBUG
			LOG_RENDER_DEBUG("VKDeviceManager: Creating DebugUtilsMessenger.");

			if (!m_Instance || std::find(m_ExtensionsEnabled.instance.begin(), m_ExtensionsEnabled.instance.end(), "VK_EXT_debug_utils")
				== m_ExtensionsEnabled.instance.end())
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
				m_DebugUtilsMessenger = m_Instance.createDebugUtilsMessengerEXT(createInfo, nullptr, m_InstanceDispatch);
				LOG_RENDER_DEBUG("VKDeviceManager: DebugUtilsMessenger created.");
			}
			catch (const vk::SystemError& err) {
				LOG_RENDER_ERROR("VKDeviceManager: Failed to create DebugUtilsMessenger: {}", err.what());
			}
#		endif
	}


	void VKDeviceManager::DestroyDebugUtilsMessenger()
	{
#		ifdef BUILD_DEBUG
			if (m_Instance && m_DebugUtilsMessenger) {
				try {
					m_Instance.destroyDebugUtilsMessengerEXT(m_DebugUtilsMessenger, nullptr, m_InstanceDispatch);
					LOG_RENDER_DEBUG("VKDeviceManager: Destroyed DebugCallback.");
				}
				catch (const vk::SystemError& err) {
					LOG_RENDER_ERROR("VKDeviceManager: Failed to destroy debug messenger: {}", err.what());
				}
				m_DebugUtilsMessenger = nullptr;
			}
#		endif
	}


	//--------------------------------------
	// vk::PhysicalDevice related functions
	//--------------------------------------


	// helper...
	std::string_view _DeviceTypeToString(vk::PhysicalDeviceType type)
	{
		switch (type)
		{
		case vk::PhysicalDeviceType::eOther: return "Other";
		case vk::PhysicalDeviceType::eIntegratedGpu: return "Integrated GPU";
		case vk::PhysicalDeviceType::eDiscreteGpu: return "Discrete GPU";
		case vk::PhysicalDeviceType::eVirtualGpu: return "Virtual GPU";
		case vk::PhysicalDeviceType::eCpu: return "CPU";
		default: return "Unknown";
		}
	}
	// ...helper


	// Enumerate all physical devices and aggregate device-level extensions into m_ExtensionsSupported.device
	void VKDeviceManager::EnumeratePhysicalDevices()
	{
		if (!m_Instance) {
			LOG_RENDER_FATAL("VKDeviceManager: Instance not created before EnumeratePhysicalDevices.");
			throw std::runtime_error("Instance not created.");
		}

		try {
			m_PhysicalDevices = m_Instance.enumeratePhysicalDevices();
		}
		catch (const vk::SystemError& err) {
			LOG_RENDER_FATAL("VKDeviceManager: Failed to enumerate physical devices: {}", err.what());
			throw;
		}

		if (m_PhysicalDevices.empty()) {
			LOG_RENDER_FATAL("VKDeviceManager: No physical devices found.");
			throw std::runtime_error("No Vulkan physical devices available.");
		}

		LOG_RENDER_DEBUG("VKDeviceManager: Found {} physical devices.", static_cast<uint32_t>(m_PhysicalDevices.size()));

		// Aggregate device-level extensions (unique) using unordered_set for O(1) checks
		std::unordered_set<std::string> uniqueExts;
		uniqueExts.reserve(m_PhysicalDevices.size() * 4);

		m_ExtensionsSupported.device.clear();
		for (const auto& dev : m_PhysicalDevices) {
			auto props = dev.getProperties();
			LOG_RENDER_DEBUG("VKDeviceManager: Device: {} (type: {})", std::string_view(props.deviceName), _DeviceTypeToString(props.deviceType));

			std::vector<vk::ExtensionProperties> devExts;
			try {
				devExts = dev.enumerateDeviceExtensionProperties();
			}
			catch (const vk::SystemError& err) {
				LOG_RENDER_WARN("VKDeviceManager: Failed to enumerate extensions for device {}: {}", std::string_view(props.deviceName), err.what());
				continue;
			}

			for (const auto& e : devExts) {
				std::string name(e.extensionName.data());
				if (uniqueExts.insert(name).second) {
					m_ExtensionsSupported.device.emplace_back(name);
					LOG_RENDER_TRACE("VKDeviceManager: Found device unique extension: {}", name);
				}
			}
		}

		LOG_RENDER_DEBUG("VKDeviceManager: Aggregated {} unique device extensions.", static_cast<uint32_t>(m_ExtensionsSupported.device.size()));
	}


	// Pick a suitable physical device (basic scoring) and cache the chosen vk::PhysicalDevice and graphics queue family
	void VKDeviceManager::PickPhysicalDevice()
	{
		if (!m_Instance) {
			LOG_RENDER_FATAL("VKDeviceManager: Instance not created before PickPhysicalDevice.");
			throw std::runtime_error("Instance not created.");
		}

		// use cached list if available
		std::vector<vk::PhysicalDevice> devices;
		if (!m_PhysicalDevices.empty()) {
			devices = m_PhysicalDevices;
		}
		else {
			try {
				devices = m_Instance.enumeratePhysicalDevices();
			}
			catch (const vk::SystemError& err) {
				LOG_RENDER_FATAL("VKDeviceManager: Failed to enumerate physical devices: {}", err.what());
				throw;
			}
		}

		if (devices.empty()) {
			LOG_RENDER_FATAL("VKDeviceManager: No physical devices found.");
			throw std::runtime_error("No Vulkan physical devices available.");
		}

		vk::PhysicalDevice bestDevice{};
		int bestScore = -1;
		uint32_t bestGraphicsIndex = UINT32_MAX;

		for (const auto& dev : devices) {
			auto props = dev.getProperties();
			auto limits = props.limits;

			// find graphics queue family index
			uint32_t gfxIndex = UINT32_MAX;
			{
				auto qprops = dev.getQueueFamilyProperties();
				for (uint32_t i = 0; i < static_cast<uint32_t>(qprops.size()); ++i) {
					if (qprops[i].queueFlags & vk::QueueFlagBits::eGraphics) {
						gfxIndex = i;
						break;
					}
				}
			}
			if (gfxIndex == UINT32_MAX) {
				LOG_RENDER_DEBUG("VKDeviceManager: Skipping device {} (no graphics queue).", std::string_view(props.deviceName));
				continue;
			}

			// enumerate device extensions for this device and check for swapchain
			bool hasSwapchain = false;
			std::vector<vk::ExtensionProperties> devExts;
			try {
				devExts = dev.enumerateDeviceExtensionProperties();
			}
			catch (const vk::SystemError& err) {
				LOG_RENDER_WARN("VKDeviceManager: Failed to enumerate device extensions for {}: {}", std::string_view(props.deviceName), err.what());
				continue;
			}
			for (const auto& e : devExts) {
				if (std::string_view(e.extensionName.data()) == "VK_KHR_swapchain") {
					hasSwapchain = true;
					break;
				}
			}
			if (!hasSwapchain) {
				LOG_RENDER_DEBUG("VKDeviceManager: Skipping device {} (no VK_KHR_swapchain).", std::string_view(props.deviceName));
				continue;
			}

			// scoring: prefer discrete GPUs and higher maxImageDimension2D
			int score = 0;
			if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += 1000;
			score += static_cast<int>(limits.maxImageDimension2D / 1024);

			LOG_RENDER_DEBUG("VKDeviceManager: Device {} (score: {}, type: {})", std::string_view(props.deviceName), score, _DeviceTypeToString(props.deviceType));

			if (score > bestScore) {
				bestScore = score;
				bestDevice = dev;
				bestGraphicsIndex = gfxIndex;
			}
		}

		if (!bestDevice) {
			LOG_RENDER_FATAL("VKDeviceManager: Failed to find a suitable GPU.");
			throw std::runtime_error("No suitable GPU found.");
		}

		m_PhysicalDevice = bestDevice;
		m_GraphicsQueueFamilyIndex = bestGraphicsIndex;

		auto chosenProps = m_PhysicalDevice.getProperties();
		LOG_RENDER_INFO("VKDeviceManager: Selected GPU: {} (type: {})", std::string_view(chosenProps.deviceName), _DeviceTypeToString(chosenProps.deviceType));

		// populate supported device extension list for the chosen device explicitly
		m_ExtensionsSupported.device.clear();
		try {
			auto chosenExts = m_PhysicalDevice.enumerateDeviceExtensionProperties();
			for (const auto& e : chosenExts) {
				std::string name(e.extensionName.data());
				if (std::find(m_ExtensionsSupported.device.begin(), m_ExtensionsSupported.device.end(), name) == m_ExtensionsSupported.device.end())
					m_ExtensionsSupported.device.emplace_back(std::move(name));
			}
		}
		catch (const vk::SystemError&) {
			// ignore: already validated presence of swapchain earlier
		}
	}


	//-------------------------------------
	// vk::LogicalDevice related functions
	//-------------------------------------


	// Create the logical device and retrieve queues
	void VKDeviceManager::CreateLogicalDevice()
	{
		if (!m_PhysicalDevice) {
			LOG_RENDER_FATAL("VKDeviceManager: Physical device not selected before CreateLogicalDevice.");
			throw std::runtime_error("Physical device not selected.");
		}

		// Ensure we have a graphics queue family index
		if (m_GraphicsQueueFamilyIndex == UINT32_MAX) {
			auto qprops = m_PhysicalDevice.getQueueFamilyProperties();
			for (uint32_t i = 0; i < static_cast<uint32_t>(qprops.size()); ++i) {
				if (qprops[i].queueFlags & vk::QueueFlagBits::eGraphics) {
					m_GraphicsQueueFamilyIndex = i;
					break;
				}
			}
			if (m_GraphicsQueueFamilyIndex == UINT32_MAX) {
				LOG_RENDER_FATAL("VKDeviceManager: No graphics queue family available.");
				throw std::runtime_error("No graphics queue family available.");
			}
		}

		// Build unique queue family list (currently only graphics; extend for present/transfer if needed)
		std::vector<uint32_t> uniqueQueueFamilies;
		uniqueQueueFamilies.push_back(m_GraphicsQueueFamilyIndex);

		// Prepare queue create infos
		float queuePriority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.reserve(uniqueQueueFamilies.size());
		for (uint32_t family : uniqueQueueFamilies) {
			vk::DeviceQueueCreateInfo qci{};
			qci.queueFamilyIndex = family;
			qci.queueCount = 1;
			qci.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(qci);
		}

		// Prepare device extensions to enable
		std::vector<std::string> requestedDeviceExts;
		// If user already requested some device extensions, use them; otherwise request swapchain if supported
		if (!m_ExtensionsEnabled.device.empty()) {
			requestedDeviceExts = m_ExtensionsEnabled.device;
		}
		else {
			// prefer swapchain
			if (IsDeviceExtensionSupported("VK_KHR_swapchain")) {
				requestedDeviceExts.emplace_back("VK_KHR_swapchain");
			}
		}

		// Convert to const char* array
		std::vector<const char*> deviceExtCStrs;
		deviceExtCStrs.reserve(requestedDeviceExts.size());
		for (const auto& s : requestedDeviceExts) deviceExtCStrs.push_back(s.c_str());

		// Query features and enable useful ones if available
		m_FeaturesSupported = m_PhysicalDevice.getFeatures();
		if (m_FeaturesSupported.samplerAnisotropy) {
			m_FeaturesEnabled.samplerAnisotropy = VK_TRUE;
			LOG_RENDER_DEBUG("VKDeviceManager: Enabling feature samplerAnisotropy on logical device.");
		}

		// Build device create info
		vk::DeviceCreateInfo createInfo{};
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.empty() ? nullptr : queueCreateInfos.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtCStrs.size());
		createInfo.ppEnabledExtensionNames = deviceExtCStrs.empty() ? nullptr : deviceExtCStrs.data();
		createInfo.pEnabledFeatures = &m_FeaturesEnabled;

		// Create the logical device
		try {
			m_Device = m_PhysicalDevice.createDevice(createInfo);
		}
		catch (const vk::SystemError& err) {
			LOG_RENDER_FATAL("VKDeviceManager: Failed to create logical device: {}", err.what());
			throw std::runtime_error("Failed to create logical device.");
		}

		// Initialize device dispatch loader for device-level extension calls (if needed)
		// NOTE: DispatchLoaderDynamic constructor expects (VkInstance, PFN_vkGetInstanceProcAddr, VkDevice, PFN_vkGetDeviceProcAddr)
		m_DeviceDispatch = vk::detail::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr, m_Device, vkGetDeviceProcAddr);
//		m_DeviceDispatch.init(m_Instance, m_Device);
		// TODO: ValidationLayer messages: Failed to find vkGetDeviceProcAddr in layer...

		// Retrieve the graphics queue
		m_GraphicsQueue = m_Device.getQueue(m_GraphicsQueueFamilyIndex, 0);

		// Remember enabled device extensions (owned strings)
		m_ExtensionsEnabled.device = requestedDeviceExts;

		LOG_RENDER_DEBUG("VKDeviceManager: Logical device created (graphics family: {}).", m_GraphicsQueueFamilyIndex);
	}


	void VKDeviceManager::DestroyLogicalDevice()
	{
		if (m_Device) {
			try {
				m_Device.waitIdle();
			}
			catch (const vk::SystemError&) {
				// proceed with best-effort destruction
			}

			try {
				m_Device.destroy();
			}
			catch (const vk::SystemError& err) {
				LOG_RENDER_ERROR("VKDeviceManager: Failed to destroy logical device: {}", err.what());
			}

			// reset handles to defaults
			m_Device = vk::Device();
			m_GraphicsQueue = vk::Queue();
			m_DeviceDispatch = vk::detail::DispatchLoaderDynamic();

			LOG_RENDER_DEBUG("VKDeviceManager: Destroyed logical device.");
		}
	}


} // namespace Helios::Engine::Renderer
