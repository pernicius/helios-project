//==============================================================================
// Vulkan Instance Wrapper
//
// Provides a RAII wrapper for a vk::Instance and a builder for its
// configuration. The VKInstance class manages the lifecycle of the Vulkan
// instance, while the VKInstanceBuilder provides a fluent interface for
// setting it up.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - VKInstanceBuilder for fluent and flexible instance configuration.
// - RAII-style management of the vk::Instance.
// - Automatic enabling of validation layers in debug builds.
// - Gathers and enables required instance extensions from the windowing system.
// - Sets up a debug messenger to log validation messages via spdlog.
// - Provides a temporary debug messenger for instance creation/destruction.
// 
// Changelog:
// - 2026.01: Added static methods to disable specific debug message IDs
// - 2026.01: Refactored to use a builder pattern
// - 2026.01: Refactored extension handling
//            Added accessors for VKDeviceManager class
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

namespace Helios::Engine::Renderer::Vulkan {


	// Struct to hold instance extensions
	struct InstanceExtensionInfo {
		std::unordered_set<std::string> required;
		std::unordered_set<std::string> optional;
		std::unordered_set<std::string> supported;
		std::vector<const char*> enabled;
	};


	class VKInstance
	{
	public:
		VKInstance(const AppSpec& appSpec, bool enableValidationLayers, InstanceExtensionInfo& extensions);
		~VKInstance();

		// Prevent copying and moving
		VKInstance(const VKInstance&) = delete;
		VKInstance& operator=(const VKInstance&) = delete;
		VKInstance(VKInstance&&) = delete;
		VKInstance& operator=(VKInstance&&) = delete;

		// Accessor for the underlying Vulkan instance
		const vk::Instance& Get() const { return m_instance; }
		operator const vk::Instance& () const { return m_instance; }

		// Accessors for VKDeviceManager class
		bool AreValidationLayersEnabled() const { return m_enableValidationLayers; }
		const std::vector<const char*>& GetValidationLayers() const { return m_validationLayers; }

		static void DisableDebugMessageID(int32_t messageId) { s_disabledMessageIds.insert(messageId); }
		static void ResetDisabledDebugMessageIDs() { s_disabledMessageIds.clear(); }

	private:

		void SetupDebugMessenger();

		// Helper functions for instance creation
		void CheckInstanceExtensionSupport();
		bool CheckValidationLayerSupport() const;

		// Debug callback
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

	private:
		vk::Instance m_instance = nullptr;
		vk::DebugUtilsMessengerEXT m_debugMessenger = nullptr;

		// Centralized list of instance extensions
		InstanceExtensionInfo m_instanceExtensionsInfo;

		const std::vector<const char*> m_validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		bool m_enableValidationLayers = false;
		static std::unordered_set<int32_t> s_disabledMessageIds;
	};


	//------------------------------------------------------------------------------
	// VKInstanceBuilder
	//------------------------------------------------------------------------------
	class VKInstanceBuilder
	{
	public:
		VKInstanceBuilder() = default;

		VKInstanceBuilder& SetAppSpec(const AppSpec& spec);
		VKInstanceBuilder& WithValidationLayers();
		VKInstanceBuilder& WithGlfwExtensions();
		VKInstanceBuilder& WithDebugMessenger();

		Scope<VKInstance> Build();

	private:
		AppSpec m_appSpec;
		InstanceExtensionInfo m_extensions;
		bool m_enableValidationLayers = false;
	};


} // namespace Helios::Engine::Renderer::Vulkan
