#pragma once

#include "Platform/Renderer/Vulkan/Misc.h"

#include <vulkan/vulkan.hpp>

namespace Helios::Engine::Renderer::Vulkan
{


	class VKInstance
	{
	public:
		VKInstance() = delete;
		VKInstance(ExtensionStruct& extensions, ExtensionStruct& layers);
		~VKInstance();

	public:
		const vk::Instance& Get() const { return m_Instance; }
		const vk::detail::DispatchLoaderDynamic& GetDispatchLoader() const { return m_DispatchLoader; }

		const ExtensionStruct& GetExtensions() const { return m_Extensions.instance; }
		const ExtensionStruct& GetLayers() const { return m_Extensions.layer; }

	private:
		void CreateInstance();
		void EnumerateExtensions();
		void EnumerateLayers();

	private:
		// ----- Debuging (VK_EXT_debug_utils)
		void InstallDebugUtilsMessenger();
		void RemoveDebugUtilsMessenger();
		vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger{};

	private:
		// core vulkan handles
		vk::Instance m_Instance{};
		vk::detail::DispatchLoaderDynamic m_DispatchLoader{};

		// extensions and layers
		struct {
			ExtensionStruct instance;
			ExtensionStruct layer;
		} m_Extensions{};
	};


} // namespace Helios::Engine::Renderer::Vulkan
