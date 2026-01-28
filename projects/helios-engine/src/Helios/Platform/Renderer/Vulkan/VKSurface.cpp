//==============================================================================
// Vulkan Surface Wrapper (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VKSurface.h
//==============================================================================
#include "pch.h"
#include "Helios/Platform/Renderer/Vulkan/VKSurface.h"

namespace Helios::Engine::Renderer::Vulkan {


	VKSurface::VKSurface(const VKInstance& instance, Window& window)
		: m_instance(instance)
	{
		// We use the C-style GLFW function here as there is no C++ equivalent in vulkan.hpp
		VkSurfaceKHR surface_c;
		VkResult result = glfwCreateWindowSurface(
			m_instance.Get(),
			static_cast<GLFWwindow*>(window.GetNativeWindow()),
			nullptr,
			&surface_c);

		if (result != VK_SUCCESS) {
			LOG_RENDER_FATAL("VKSurface: Failed to create window surface!");
		}

		m_surface = vk::SurfaceKHR(surface_c);
		LOG_RENDER_DEBUG("VKSurface: Window surface created.");
	}


	VKSurface::~VKSurface()
	{
		if (m_surface) {
			m_instance.Get().destroySurfaceKHR(m_surface);
			LOG_RENDER_DEBUG("VKSurface: Window surface destroyed.");
		}
	}


} // namespace Helios::Engine::Renderer::Vulkan
