#pragma once

#include "Helios/Engine/Renderer/DeviceManager.h"

#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace Helios::Engine::Renderer
{


	class VKDeviceManager : public DeviceManager
	{
	public:
		VKDeviceManager();
		~VKDeviceManager();

		// --------------------------------------------------
		// Inherited via DeviceManager
		// --------------------------------------------------

		void Init() override;

	public:
		// --------------------------------------------------
		// Vulkan specific
		// --------------------------------------------------

		// ----- vk::Instance management -----

		void CreateInstance();
		void DestroyInstance();
		vk::Instance GetVKInstance() const { return m_Instance; }

		void EnumerateInstanceExtensionNames();
		inline bool IsInstanceExtensionSupported(const char* extension) const {
			return std::any_of(m_ExtensionsSupported.instance.begin(), m_ExtensionsSupported.instance.end(),
				[extension](const std::string& s) { return s == extension; });
		}
		inline bool IsInstanceExtensionEnabled(const char* extension) const {
			return std::any_of(m_ExtensionsEnabled.instance.begin(), m_ExtensionsEnabled.instance.end(),
				[extension](const std::string& s) { return s == extension; });
		}

		void EnumerateInstanceLayerNames();
		inline bool IsLayerSupported(const char* layer) const {
			return std::any_of(m_ExtensionsSupported.layer.begin(), m_ExtensionsSupported.layer.end(),
				[layer](const std::string& s) { return s == layer; });
		}
		inline bool IsLayerEnabled(const char* layer) const {
			return std::any_of(m_ExtensionsEnabled.layer.begin(), m_ExtensionsEnabled.layer.end(),
				[layer](const std::string& s) { return s == layer; });
		}

		inline bool IsDeviceExtensionSupported(const char* extension) const {
			return std::any_of(m_ExtensionsSupported.device.begin(), m_ExtensionsSupported.device.end(),
				[extension](const std::string& s) { return s == extension; });
		}
		inline bool IsDeviceExtensionEnabled(const char* extension) const {
			return std::any_of(m_ExtensionsEnabled.device.begin(), m_ExtensionsEnabled.device.end(),
				[extension](const std::string& s) { return s == extension; });
		}

		// ----- vk::PhysicalDevice management -----

		void EnumeratePhysicalDevices();
		void PickPhysicalDevice();

		// ----- vk::LogicalDevice management -----
		void CreateLogicalDevice();
		void DestroyLogicalDevice();

	private:
		// Debug utils (VK_EXT_debug_utils)
		void SetupDebugUtilsMessenger();
		void DestroyDebugUtilsMessenger();
		vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger{};

	private:
		// core vulkan handles
		vk::Instance m_Instance{};
		vk::PhysicalDevice m_PhysicalDevice{};
		vk::Device m_Device{};
		vk::Queue m_GraphicsQueue{};

		vk::detail::DispatchLoaderDynamic m_InstanceDispatch{};
		vk::detail::DispatchLoaderDynamic m_DeviceDispatch{};

		// selected device information
		uint32_t m_GraphicsQueueFamilyIndex = UINT32_MAX;

		// cached extensions and layers
		struct ExtensionSet {
			std::vector<std::string> instance;
			std::vector<std::string> layer;
			std::vector<std::string> device;
		};
		ExtensionSet m_ExtensionsSupported;
		ExtensionSet m_ExtensionsEnabled;
		vk::PhysicalDeviceFeatures m_FeaturesSupported{};
		vk::PhysicalDeviceFeatures m_FeaturesEnabled{};

		// cached physical device list (avoid re-enumeration)
		std::vector<vk::PhysicalDevice> m_PhysicalDevices;
	};


} // namespace Helios::Engine::Renderer
