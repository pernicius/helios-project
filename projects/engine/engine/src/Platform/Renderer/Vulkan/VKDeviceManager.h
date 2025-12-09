#pragma once

#include "Helios/Engine/Renderer/DeviceManager.h"

#include "Platform/Renderer/Vulkan/VKInstance.h"
#include "Platform/Renderer/Vulkan/VKDevice.h"

#include <vulkan/vulkan.hpp>

namespace Helios::Engine::Renderer::Vulkan
{


	class VKDeviceManager : public DeviceManager
	{
	public:
		VKDeviceManager();
		~VKDeviceManager();

	public:
		Ref<VKInstance> GetInstance() const { return m_Instance; }
		vk::detail::DispatchLoaderDynamic GetInstanceLoader() const { return m_Instance->GetDispatchLoader(); }

#if 0
//		VKDevice GetDevice() const { return m_Device; }
//
//		vk::detail::DispatchLoaderDynamic GetDeviceLoader() const { return m_DeviceDispatch; }
//
//		// Device selection configuration
//		void SetSelectionCriteria(const DeviceSelectionCriteria& criteria) { m_SelectionCriteria = criteria; }
//		const DeviceSelectionCriteria& GetSelectionCriteria() const { return m_SelectionCriteria; }
#endif

	private:
		void Init();

	private:
		Ref<Window> m_Window;

		Ref<VKInstance> m_Instance;
		Ref<VKDevice> m_Device;

		// extensions and layers
		struct {
			ExtensionStruct instance;
			ExtensionStruct layer;
			ExtensionStruct device;
		} m_Extensions{};

#if 0
		// core vulkan handles

		vk::detail::DispatchLoaderDynamic m_InstanceDispatch{};
		vk::detail::DispatchLoaderDynamic m_DeviceDispatch{};

		// device features
		vk::PhysicalDeviceFeatures m_FeaturesSupported{};
		vk::PhysicalDeviceFeatures m_FeaturesEnabled{};
#endif
	};


} // namespace Helios::Engine::Renderer::Vulkan
