//==============================================================================
// Vulkan API Support Detection (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VKSupport.h
//==============================================================================
#include "pch.h"
#include "Helios/Platform/Renderer/Vulkan/VKSupport.h"

#include <vulkan/vulkan.hpp>

#include <mutex>

namespace Helios::Engine::Renderer::Vulkan
{

	bool IsAPISupported()
	{
		static std::once_flag s_flag;
		static bool s_result = false;

		std::call_once(s_flag, []()
		{
			// Initialize GLFW and check for Vulkan support
			if (!glfwInit()) {
				s_result = false;
				return;
			}
			if (!glfwVulkanSupported()) {
				s_result = false;
				LOG_RENDER_INFO("Vulkan is not supported (GLFW reports no support).");
				return;
			}

			// Try to enumerate Vulkan instance version
			try
			{
				uint32_t version = 0;
				vk::Result result = vk::enumerateInstanceVersion(&version);
				if (result != vk::Result::eSuccess) {
					s_result = false;
					LOG_RENDER_INFO("Vulkan is not supported (enumerateInstanceVersion failed).");
					return;
				}
				s_result = true;
				LOG_RENDER_INFO("Vulkan max supported version: {}.{}.{}", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));

#if 0
				vk::ApplicationInfo appInfo{};
				appInfo.pApplicationName = "VulkanSupportCheck";
				appInfo.applicationVersion = 1;
				appInfo.pEngineName = "Helios";
				appInfo.engineVersion = 1;
				appInfo.apiVersion = VK_API_VERSION_1_0;
//				appInfo.apiVersion = VK_API_VERSION_1_1;
//				appInfo.apiVersion = VK_API_VERSION_1_2;
//				appInfo.apiVersion = VK_API_VERSION_1_3;

				vk::InstanceCreateInfo createInfo{};
				createInfo.pApplicationInfo = &appInfo;

				vk::UniqueInstance instance = vk::createInstanceUnique(createInfo);
				s_result = true;
				LOG_RENDER_INFO("Vulkan is supported.");
#endif
			}
			catch (const vk::SystemError&) {
				s_result = false;
				LOG_RENDER_INFO("Vulkan is not supported (SystemError).");
			}
			
			//----------------------------------------
			// TODO: check extensions required by GLFW
			//----------------------------------------
		});

		return s_result;
	}

} // namespace Helios::Engine::Renderer::Vulkan
