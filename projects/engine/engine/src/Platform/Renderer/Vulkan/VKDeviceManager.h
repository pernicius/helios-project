#pragma once

#include "Helios/Engine/Renderer/DeviceManager.h"

#include <vulkan/vulkan.hpp>

namespace Helios::Engine::Renderer
{


	class VKDeviceManager : public DeviceManager
	{
	public:
		VKDeviceManager();
		~VKDeviceManager();

		// Inherited via DeviceManager
		void Init() override;

	public:
		// Vulkan specific
		vk::Instance GetVKInstance() { return m_Instance; }

		void EnumerateInstanceExtensionNames();
		inline bool IsExtensionSupported(const char* extension) const {
			return m_ExtensionsSupported.find(extension) != m_ExtensionsSupported.end();
		}
		inline bool IsExtensionEnabled(const char* extension) const {
			return std::any_of(m_ExtensionsEnabled.begin(), m_ExtensionsEnabled.end(),
				[extension](const std::string& s) { return s == extension; });
		}

		void EnumerateInstanceLayerNames();
		inline bool IsLayerSupported(const char* layer) const {
			return m_LayersSupported.find(layer) != m_LayersSupported.end();
		}
		inline bool IsLayerEnabled(const char* layer) const {
			return std::any_of(m_LayersEnabled.begin(), m_LayersEnabled.end(),
				[layer](const std::string& s) { return s == layer; });
		}

	private:
		void CreateInstance();
		void DestroyInstance();

	private:
		// Debug utils (VK_EXT_debug_utils)
		void SetupDebugMessenger();
		void DestroyDebugMessenger();
		vk::DebugUtilsMessengerEXT m_DebugMessenger{};

	private:
		vk::Instance m_Instance{};
		vk::detail::DispatchLoaderDynamic m_DispatchLoader{};

		// cached supported names
		std::unordered_set<std::string> m_ExtensionsSupported;
		std::unordered_set<std::string> m_LayersSupported;

		// enabled lists (owned strings to avoid dangling pointers)
		std::vector<std::string> m_ExtensionsEnabled;
		std::vector<std::string> m_LayersEnabled;

	};


} // namespace Helios::Engine::Renderer
