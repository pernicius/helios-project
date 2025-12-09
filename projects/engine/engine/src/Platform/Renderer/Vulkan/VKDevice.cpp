#include "pch.h"
#include "Platform/Renderer/Vulkan/VKDevice.h"

namespace Helios::Engine::Renderer::Vulkan {


	//-------------------------------
	// Helper functions used locally
	//-------------------------------


	// Local helper: Create a unique cache key for a physical device and surface combination
	static uint64_t MakeQueueCacheKey(vk::PhysicalDevice pd, vk::SurfaceKHR surface)
	{
		auto devHandle = reinterpret_cast<uint64_t>(static_cast<VkPhysicalDevice>(pd));
		auto surfHandle = static_cast<VkSurfaceKHR>(surface) ? reinterpret_cast<uint64_t>(static_cast<VkSurfaceKHR>(surface)) : 0ull;
		// mix bits
		return devHandle ^ ((surfHandle << 1) | 0x9e3779b97f4a7c15ull);
	}


	// Local helper: Convert vk::PhysicalDeviceType to string
	static std::string_view DeviceTypeToString(vk::PhysicalDeviceType type)
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


	//-------------------------------------
	// VKDevice constructor and destructor
	//-------------------------------------

	VKDevice::VKDevice(Ref<VKInstance> instance, ExtensionStruct& extensions)
		: m_Instance(instance)
	{
		// store requested extensions and layers
		m_Extensions.device = extensions;
		// update with extensions required by SelectionCriteria
		if (m_SelectionCriteria.requireSwapchain && !m_Extensions.device.required.contains("VK_KHR_swapchain")) {
			m_Extensions.device.required.insert("VK_KHR_swapchain");
		}

		PickPhysicalDevice();
		CreateLogicalDevice();
	}
	
	
	VKDevice::~VKDevice()
	{
		if (m_LogicalDevice) {
			try {
				m_LogicalDevice.waitIdle();
			}
			catch (const vk::SystemError&) {
				// proceed with best-effort destruction
			}
			m_LogicalDevice.destroy();
			m_LogicalDevice = vk::Device();
			m_Queues = {};
			m_DispatchLoader = vk::detail::DispatchLoaderDynamic();
			LOG_RENDER_DEBUG("VKDevice: Destroyed logical device.");
		}

		if (m_PhysicalDevice) {
			// Physical devices are implicitly destroyed with the instance; nothing to do here.
			m_PhysicalDevice = vk::PhysicalDevice();
			LOG_RENDER_DEBUG("VKDevice: Unselected physical device.");
		}
	}


	// ------------------------------------------
	// Physical device selection and enumeration
	// ------------------------------------------


	void VKDevice::PickPhysicalDevice()
	{
		LOG_RENDER_DEBUG("VKDevice: Selecting physical device.");

		// use cached device list if available
		std::vector<vk::PhysicalDevice> devices;
		if (!m_PhysicalDevices.empty()) {
			devices = m_PhysicalDevices;
		}
		else {
			try {
				devices = m_Instance->Get().enumeratePhysicalDevices();
			}
			catch (const vk::SystemError& err) {
				LOG_RENDER_FATAL("VKDevice: Failed to enumerate physical devices: {}", err.what());
				throw;
			}
		}
		if (devices.empty()) {
			LOG_RENDER_FATAL("VKDevice: No physical devices found.");
			throw std::runtime_error("No Vulkan physical devices available.");
		}

		// prepare for scoring
		const DeviceSelectionCriteria& criteria = m_SelectionCriteria;
		vk::PhysicalDevice bestDevice{};
		int bestScore = -1;
		QueueFamilyIndices bestQueueFamilyIndices{};

		// evaluate each device
		for (const auto& dev : devices) {
			auto props = dev.getProperties();
			auto limits = props.limits;

			// find queue families using helper (graphics, present, compute, transfer)
			auto qindices = FindQueueFamilies(dev, m_Surface);

			if (!qindices.graphics.index.has_value()) {
				LOG_RENDER_DEBUG("VKDevice: Skipping device {} (no graphics queue).", std::string_view(props.deviceName));
				continue;
			}
			uint32_t gfxIndex = qindices.graphics.index.value();

			// enumerate device extensions for this device and check for swapchain & required extensions
			bool hasSwapchain = false;
			std::vector<vk::ExtensionProperties> devExts;
			try {
				devExts = dev.enumerateDeviceExtensionProperties();
			}
			catch (const vk::SystemError& err) {
				LOG_RENDER_WARN("VKDevice: Failed to enumerate device extensions for {}: {}", std::string_view(props.deviceName), err.what());
				continue;
			}

			// build quick-access list of extension names
			std::unordered_set<std::string> devExtNames;
			devExtNames.reserve(devExts.size());
			for (const auto& e : devExts) {
				devExtNames.insert(e.extensionName.data());
				if (std::string_view(e.extensionName.data()) == "VK_KHR_swapchain") hasSwapchain = true;
			}

			// enforce criteria: require swapchain if requested
			if (criteria.requireSwapchain && !hasSwapchain) {
				LOG_RENDER_DEBUG("VKDevice: Skipping device {} (no VK_KHR_swapchain).", std::string_view(props.deviceName));
				continue;
			}

			// enforce required extensions
			bool missingRequired = false;
			for (const auto& req : m_Extensions.device.required) {
				if (!devExtNames.contains(req)) {
					LOG_RENDER_DEBUG("VKDevice: Skipping device {} (missing required extension {}).", std::string_view(props.deviceName), req);
					missingRequired = true;
					break;
				}
			}
			if (missingRequired) continue;

			// scoring: prefer discrete GPUs and higher maxImageDimension2D
			int score = 0;
			if (criteria.preferDiscrete && props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += criteria.discreteWeight;
			score += static_cast<int>(limits.maxImageDimension2D / 1024) * criteria.imageDimWeight;

			// bonus for preferred extensions
			for (const auto& pref : m_Extensions.device.optional) {
				if (devExtNames.contains(pref)) score += criteria.optionalExtensionWeight;
			}

			// prefer devices with separate dedicated compute/transfer families
			if (qindices.compute.index.has_value() && qindices.compute.index.value() != gfxIndex && qindices.compute.dedicatedCompute)
				score += 50;
			if (qindices.transfer.index.has_value() && qindices.transfer.index.value() != gfxIndex && qindices.transfer.dedicatedTransfer)
				score += 30;

			// prefer devices with a separate present family (optional tie-breaker)
			if (qindices.present.index.has_value() && qindices.present.index.value() != gfxIndex) {
				// small bonus for having separate present family (can be beneficial on some platforms)
				score += 10;
			}

			LOG_RENDER_DEBUG("VKDevice: Device {} (score: {}, type: {})", std::string_view(props.deviceName), score, DeviceTypeToString(props.deviceType));

			if (score > bestScore) {
				bestScore = score;
				bestDevice = dev;
				bestQueueFamilyIndices = qindices;
			}
		}

		if (!bestDevice) {
			LOG_RENDER_FATAL("VKDevice: Failed to find a suitable GPU.");
			throw std::runtime_error("No suitable GPU found.");
		}

		m_PhysicalDevice = bestDevice;
		m_QueueFamilyIndices = bestQueueFamilyIndices;

		auto chosenProps = m_PhysicalDevice.getProperties();
		LOG_RENDER_INFO("Selected GPU: {} (type: {})", std::string_view(chosenProps.deviceName), DeviceTypeToString(chosenProps.deviceType));

		// populate supported device extension list for the chosen device explicitly
		m_Extensions.device.supported.clear();
		try {
			auto chosenExts = m_PhysicalDevice.enumerateDeviceExtensionProperties();
			for (const auto& e : chosenExts) {
				std::string name(e.extensionName.data());
				if (!m_Extensions.device.supported.contains(name))
					m_Extensions.device.supported.insert(std::move(name));
			}
		}
		catch (const vk::SystemError&) {
			// ignore: already validated presence of swapchain earlier
		}
	}


	VKDevice::QueueFamilyIndices VKDevice::FindQueueFamilies(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
	{
		// caching
		uint64_t key = MakeQueueCacheKey(physicalDevice, surface);
		auto it = m_QueueFamilyCache.find(key);
		if (it != m_QueueFamilyCache.end()) return it->second;

		QueueFamilyIndices indices{};
		auto qprops = physicalDevice.getQueueFamilyProperties();

		for (uint32_t i = 0; i < static_cast<uint32_t>(qprops.size()); ++i) {
			const auto& props = qprops[i];

			QueueFamilyInfo info{};
			info.index = i;
			info.flags = props.queueFlags;
			info.queueCount = props.queueCount;
			info.timestampValidBits = props.timestampValidBits;

			// dedications
			const bool isGraphics = static_cast<bool>(props.queueFlags & vk::QueueFlagBits::eGraphics);
			const bool isCompute = static_cast<bool>(props.queueFlags & vk::QueueFlagBits::eCompute);
			const bool isTransfer = static_cast<bool>(props.queueFlags & vk::QueueFlagBits::eTransfer);

			info.dedicatedCompute = (isCompute && !isGraphics);
			info.dedicatedTransfer = (isTransfer && !isGraphics && !isCompute);

			// present support (prefer vk::SurfaceKHR query; fall back to GLFW helper)
			bool presentSupported = false;
			if (surface) {
				try {
					presentSupported = static_cast<bool>(physicalDevice.getSurfaceSupportKHR(i, surface, m_Instance->GetDispatchLoader()));
				}
				catch (const vk::SystemError&) {
					// fallback to glfw if dispatch isn't available
					presentSupported = static_cast<bool>(glfwGetPhysicalDevicePresentationSupport(m_Instance->Get(), physicalDevice, i));
				}
			}
			else {
				// if no surface provided, use glfw helper as we have no surface to query against
				presentSupported = static_cast<bool>(glfwGetPhysicalDevicePresentationSupport(m_Instance->Get(), physicalDevice, i));
			}
			info.presentSupport = presentSupported;

			// heuristic score for family (for selection)
			int h = 0;
			if (isGraphics) h += 200;
			if (info.dedicatedCompute) h += 50;
			if (info.dedicatedTransfer) h += 30;
			h += static_cast<int>(info.timestampValidBits);
			info.heuristicScore = h;

			// assign to the best matching slot
			if (isGraphics && !indices.graphics.index.has_value()) indices.graphics = info;
			if (info.presentSupport && !indices.present.index.has_value()) indices.present = info;
			// prefer dedicated compute if possible
			if (isCompute) {
				if (!indices.compute.index.has_value() || info.dedicatedCompute)
					indices.compute = info;
			}
			// prefer dedicated transfer if possible
			if (isTransfer) {
				if (!indices.transfer.index.has_value() || info.dedicatedTransfer)
					indices.transfer = info;
			}
		}

		// cache and return
		m_QueueFamilyCache.emplace(key, indices);
		return m_QueueFamilyCache[key];
	}


	//-------------------------
	// Logical device creation
	//-------------------------


	void VKDevice::CreateLogicalDevice()
	{
		LOG_RENDER_DEBUG("VKDevice: Creating logical device.");

		// gather unique queue family indices we need
		std::vector<uint32_t> uniqueFamilies;
		auto pushIf = [&](const std::optional<uint32_t>& opt) {
			if (opt.has_value()) uniqueFamilies.push_back(opt.value());
			};
		pushIf(m_QueueFamilyIndices.graphics.index);
		pushIf(m_QueueFamilyIndices.present.index);
		pushIf(m_QueueFamilyIndices.compute.index);
		pushIf(m_QueueFamilyIndices.transfer.index);

		std::sort(uniqueFamilies.begin(), uniqueFamilies.end());
		uniqueFamilies.erase(std::unique(uniqueFamilies.begin(), uniqueFamilies.end()), uniqueFamilies.end());

		// prepare queue create infos
		float queuePriority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.reserve(uniqueFamilies.size());
		for (uint32_t family : uniqueFamilies) {
			vk::DeviceQueueCreateInfo qci{};
			qci.queueFamilyIndex = family;
			qci.queueCount = 1;
			qci.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(qci);
		}

		// finalize device extension list (ensure required are present)
		m_Extensions.device.enabled.clear();
		for (const auto& req : m_Extensions.device.required) {
			if (m_Extensions.device.supported.contains(req)) {
				if (!m_Extensions.device.enabled.contains(req)) {
					m_Extensions.device.enabled.insert(req);
					LOG_RENDER_DEBUG("VKDevice: Required logical device extension: [ OK ]: {}", req);
				}
			}
			else {
				LOG_RENDER_FATAL("VKDevice: Required logical device extension: [FAIL]: {}", req);
				throw std::runtime_error("Required device extension not supported.");
			}
		}
		// enable optional if supported
		for (const auto& opt : m_Extensions.device.optional) {
			if (m_Extensions.device.supported.contains(opt)) {
				if (!m_Extensions.device.enabled.contains(opt)) {
					m_Extensions.device.enabled.insert(opt);
					LOG_RENDER_DEBUG("VKDevice: Optional logical device extension: [ OK ]: {}", opt);
				}
			}
			else
				LOG_RENDER_DEBUG("VKDevice: Optional logical device extension: [FAIL]: {}", opt);
		}

		// prepare raw pointers for vk create info
		std::vector<const char*> deviceExtCStrs;
		deviceExtCStrs.reserve(m_Extensions.device.enabled.size());
		for (const auto& s : m_Extensions.device.enabled) deviceExtCStrs.push_back(s.c_str());

		// prepare device features (request only what is supported)
		vk::PhysicalDeviceFeatures supportedFeatures = m_PhysicalDevice.getFeatures();
		vk::PhysicalDeviceFeatures enabledFeatures{};
		if (supportedFeatures.samplerAnisotropy) { enabledFeatures.samplerAnisotropy = true; LOG_RENDER_DEBUG("VKDevice: Enabled logical device feature: samplerAnisotropy"); }
		if (supportedFeatures.sampleRateShading) { enabledFeatures.sampleRateShading = true; LOG_RENDER_DEBUG("VKDevice: Enabled logical device feature: sampleRateShading"); }
		// add other feature requests as needed, guarded by supportedFeatures

		// setup device create info
		vk::DeviceCreateInfo createInfo{};
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.empty() ? nullptr : queueCreateInfos.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtCStrs.size());
		createInfo.ppEnabledExtensionNames = deviceExtCStrs.empty() ? nullptr : deviceExtCStrs.data();
		createInfo.pEnabledFeatures = &enabledFeatures;

		// finally try to create the device
		try {
			m_LogicalDevice = m_PhysicalDevice.createDevice(createInfo);
		}
		catch (const vk::SystemError& err) {
			LOG_RENDER_FATAL("VKDevice: Failed to create logical device: {}", err.what());
			throw std::runtime_error("Failed to create logical device.");
		}

		// retrieve queues we care about (may be same handle for multiple roles)
		if (m_QueueFamilyIndices.graphics.index.has_value())
			m_Queues.graphics = m_LogicalDevice.getQueue(m_QueueFamilyIndices.graphics.index.value(), 0);
		if (m_QueueFamilyIndices.present.index.has_value())
			m_Queues.present = m_LogicalDevice.getQueue(m_QueueFamilyIndices.present.index.value(), 0);
		if (m_QueueFamilyIndices.compute.index.has_value())
			m_Queues.compute = m_LogicalDevice.getQueue(m_QueueFamilyIndices.compute.index.value(), 0);
		if (m_QueueFamilyIndices.transfer.index.has_value())
			m_Queues.transfer = m_LogicalDevice.getQueue(m_QueueFamilyIndices.transfer.index.value(), 0);

		// get dispatch loader using vkGetInstanceProcAddr so extension entry points are resolved
		m_DispatchLoader = vk::detail::DispatchLoaderDynamic(m_Instance->Get(), vkGetInstanceProcAddr, m_LogicalDevice, vkGetDeviceProcAddr);
	}


} // namespace Helios::Engine::Renderer::Vulkan

