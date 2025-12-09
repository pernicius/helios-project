#include "pch.h"
#include "Platform/Renderer/Vulkan/VKInstance.h"

#include "Helios/Engine/Spec/Spec.h"

namespace Helios::Engine::Renderer::Vulkan {


	//-------------------------------
	// Helper functions used locally
	//-------------------------------


	static uint32_t QuerySupportedInstanceVersion()
	{
		uint32_t version = VK_API_VERSION_1_0;
		// enumerateInstanceVersion is available on recent loaders; if it throws, fallback to 1.0
		try {
			version = vk::enumerateInstanceVersion();
		}
		catch (const vk::SystemError& e) {
			LOG_RENDER_WARN("VKInstance: enumerateInstanceVersion not available, falling back to 1.0: {}", e.what());
			// keep version = 1.0
		}
		return version;
	}


	// Log & verify helpers to reduce duplication
	static bool VerifyRequiredSupport(const char* kind, const std::unordered_set<std::string>& required, const std::unordered_set<std::string>& supported)
	{
		bool allSupported = true;
		for (const auto& r : required) {
			if (supported.contains(r)) {
				LOG_RENDER_DEBUG("VKInstance: Required {} [ OK ]: {}", kind, r);
			}
			else {
				LOG_RENDER_ERROR("VKInstance: Required {} [FAIL]: {}", kind, r);
				allSupported = false;
			}
		}
		return allSupported;
	}

	static void ProcessOptionalSupport(const char* kind, const std::unordered_set<std::string>& optional, std::unordered_set<std::string>& enabled, const std::unordered_set<std::string>& supported)
	{
		for (const auto& o : optional) {
			if (supported.contains(o)) {
				LOG_RENDER_DEBUG("VKInstance: Optional {} [ OK ]: {}", kind, o);
				if (!enabled.contains(o))
					enabled.insert(o);
			}
			else {
				LOG_RENDER_DEBUG("VKInstance: Optional {} [FAIL]: {}", kind, o);
			}
		}
	}


	//---------------------------------------
	// VKInstance constructor and destructor
	//---------------------------------------


	VKInstance::VKInstance(ExtensionStruct& extensions, ExtensionStruct& layers)
	{
		// store requested extensions and layers
		m_Extensions.instance = extensions;
		m_Extensions.layer = layers;

		CreateInstance();
		InstallDebugUtilsMessenger();
	}


	VKInstance::~VKInstance()
	{
		RemoveDebugUtilsMessenger();

		if (m_Instance) {
			m_Instance.destroy();
			LOG_RENDER_DEBUG("VKInstance: Destroyed instance.");
		}
	}


	//-------------------
	// instance creation
	//-------------------


	void VKInstance::CreateInstance()
	{
		LOG_RENDER_DEBUG("VKInstance: Creating instance.");

		// check vulkan loader/runtime version and pick an API version we can use.
		const uint32_t requestedApiVersion = VK_API_VERSION_1_4;
		const uint32_t supportedApiVersion = QuerySupportedInstanceVersion();
		const uint32_t chosenApiVersion = std::min(requestedApiVersion, supportedApiVersion);

		if (chosenApiVersion < VK_API_VERSION_1_0) {
			LOG_RENDER_FATAL("Vulkan loader reports no usable instance API version.");
			throw std::runtime_error("Vulkan loader reports no usable instance API version.");
		}

		// setup application info
		vk::ApplicationInfo appInfo{};
		appInfo.pApplicationName = Spec::App::Name.c_str();
		appInfo.applicationVersion = Spec::App::Version;
		appInfo.pEngineName = "Helios";
		appInfo.engineVersion = HE_VERSION;
		appInfo.apiVersion = chosenApiVersion;

		// cache supported instance extensions early so we only request those once
		EnumerateExtensions();

		// get required extensions from GLFW
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		if (!glfwExtensions || glfwExtensionCount == 0) {
			LOG_RENDER_FATAL("GLFW did not return any required instance extensions.");
			throw std::runtime_error("GLFW did not return any required instance extensions.");
		}
		// add GLFW required extensions to required set
		for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
			if (!m_Extensions.instance.required.contains(glfwExtensions[i]))
				m_Extensions.instance.required.insert(glfwExtensions[i]);
		}
		// check required extension support
		LOG_RENDER_DEBUG("VKInstance: Checking required instance extensions...");
		if (!VerifyRequiredSupport("instance extension", m_Extensions.instance.required, m_Extensions.instance.supported)) {
			LOG_RENDER_FATAL("Required vulkan instance extensions not supported!");
			throw std::runtime_error("Required vulkan instance extensions not supported!");
		}
		// store enabled extensions
		m_Extensions.instance.enabled = m_Extensions.instance.required;

		// check optional extension support
#		ifdef BUILD_DEBUG
			// add debug utils extension to required extensions in debug builds
		if (!m_Extensions.instance.optional.contains("VK_EXT_debug_utils"))
			m_Extensions.instance.optional.insert("VK_EXT_debug_utils");
#		endif
		LOG_RENDER_DEBUG("VKInstance: Checking optional instance extensions...");
		ProcessOptionalSupport("instance extension", m_Extensions.instance.optional, m_Extensions.instance.enabled, m_Extensions.instance.supported);
//#		ifdef BUILD_DEBUG
//			// list unused extensions
//			for (const auto& ext : m_Extensions.instance.supported) {
//				if (!m_Extensions.instance.enabled.contains(ext))
//					LOG_RENDER_DEBUG("VKInstance: Unused instance extension   [INFO]: {}", ext);
//			}
//#		endif

		// log summary
		LOG_RENDER_DEBUG("VKInstance: Done ({} of {} instance extensions enabled).", m_Extensions.instance.enabled.size(), m_Extensions.instance.supported.size());

		// cache supported instance layers early so we only request those once
		EnumerateLayers();
		// check required layer support
		LOG_RENDER_DEBUG("VKInstance: Checking required instance layers...");
		if (!VerifyRequiredSupport("instance layer    ", m_Extensions.layer.required, m_Extensions.layer.supported)) {
			LOG_RENDER_FATAL("Required vulkan instance layers not supported!");
			throw std::runtime_error("Required vulkan instance layers not supported!");
		}
		// store enabled layers
		m_Extensions.layer.enabled = m_Extensions.layer.required;

		// check optional layer support
#		ifdef BUILD_DEBUG
		if (!m_Extensions.layer.optional.contains("VK_LAYER_KHRONOS_validation"))
			m_Extensions.layer.optional.insert("VK_LAYER_KHRONOS_validation");
#		endif
		LOG_RENDER_DEBUG("VKInstance: Checking optional instance layers...");
		ProcessOptionalSupport("instance layer    ", m_Extensions.layer.optional, m_Extensions.layer.enabled, m_Extensions.layer.supported);
//#		ifdef BUILD_DEBUG
//			// list unused layers
//			for (const auto& ext : m_Extensions.layer.supported) {
//				if (!m_Extensions.layer.enabled.contains(ext))
//					LOG_RENDER_DEBUG("VKInstance: Unused instance layer       [INFO]: {}", ext);
//			}
//#		endif

		// log summary
		LOG_RENDER_DEBUG("VKInstance: Done ({} of {} instance layers enabled).", m_Extensions.layer.enabled.size(), m_Extensions.layer.supported.size());

		// prepare raw pointers for vk create info
		std::vector<const char*> extCStrs;
		extCStrs.reserve(m_Extensions.instance.enabled.size());
		for (const auto& s : m_Extensions.instance.enabled) extCStrs.push_back(s.c_str());

		std::vector<const char*> layerCStrs;
		layerCStrs.reserve(m_Extensions.layer.enabled.size());
		for (const auto& s : m_Extensions.layer.enabled) layerCStrs.push_back(s.c_str());

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

		// get dispatch loader using vkGetInstanceProcAddr so extension entry points are resolved
		m_DispatchLoader = vk::detail::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr);
	}


	void VKInstance::EnumerateExtensions()
	{
		auto props = vk::enumerateInstanceExtensionProperties();
		m_Extensions.instance.supported.clear();
		m_Extensions.instance.supported.reserve(props.size());
		for (const auto& p : props) {
			m_Extensions.instance.supported.insert(std::string(p.extensionName.data()));
//			LOG_RENDER_TRACE("VKInstance: Found instance extension: {}", p.extensionName.data());
		}
	}


	void VKInstance::EnumerateLayers()
	{
		auto props = vk::enumerateInstanceLayerProperties();
		m_Extensions.layer.supported.clear();
		m_Extensions.layer.supported.reserve(props.size());
		for (const auto& p : props) {
			m_Extensions.layer.supported.insert(std::string(p.layerName.data()));
//			LOG_RENDER_TRACE("VKInstance: Found instance layer: {}", p.layerName.data());
		}
	}

} // namespace Helios::Engine::Renderer::Vulkan
