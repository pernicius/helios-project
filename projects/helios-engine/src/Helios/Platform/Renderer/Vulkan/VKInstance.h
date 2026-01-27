//==============================================================================
// Vulkan Instance Wrapper
//
// Provides a RAII (Resource Acquisition Is Initialization) wrapper for a
// vk::Instance, handling its creation and destruction. It is responsible for
// gathering required extensions (e.g., from GLFW), enabling validation layers
// for debugging, and setting up the debug messenger for validation output.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - RAII-style management of the vk::Instance.
// - Automatic enabling of validation layers in debug builds.
// - Gathers and enables required instance extensions from the windowing system.
// - Sets up a debug messenger to log validation messages via spdlog.
// - Provides a temporary debug messenger for instance creation/destruction.
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include "Helios/Engine/Core/Application.h"

namespace Helios::Engine::Renderer::Vulkan {


	class VKInstance
	{
	public:
		VKInstance(const AppSpec& appSpec);
		~VKInstance();

		// Prevent copying and moving
		VKInstance(const VKInstance&) = delete;
		VKInstance& operator=(const VKInstance&) = delete;
		VKInstance(VKInstance&&) = delete;
		VKInstance& operator=(VKInstance&&) = delete;

		// Accessor for the underlying Vulkan instance
		const vk::Instance& Get() const { return m_instance; }
		operator const vk::Instance& () const { return m_instance; }

	private:
		void CreateInstance(const AppSpec& appSpec);
		void SetupDebugMessenger();

		// Helper functions for instance creation
		std::vector<const char*> GetRequiredExtensions() const;
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

		const std::vector<const char*> m_validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

#		ifdef HE_DEBUG
			const bool m_enableValidationLayers = true;
#		else
			const bool m_enableValidationLayers = false;
#		endif
	};


} // namespace Helios::Engine::Renderer::Vulkan
