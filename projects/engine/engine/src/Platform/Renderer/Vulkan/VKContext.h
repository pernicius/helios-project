#pragma once

#include "Helios/Engine/Renderer/Context.h"

#include <vulkan/vulkan.hpp>

namespace Helios::Engine::Renderer
{


	class VKContext : public Context
	{
	public:
		VKContext();
		~VKContext();

		void Init();

	public:
		vk::Instance GetVKInstance() { return m_Instance; }

	private:
		bool CheckExtensionsSupport(const std::vector<const char*>& requiredExtensions);
		bool CheckLayerSupport(const std::vector<const char*>& requiredLayers);

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
	};


} // namespace Helios::Engine::Renderer
