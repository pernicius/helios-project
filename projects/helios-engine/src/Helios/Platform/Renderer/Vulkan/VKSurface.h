//==============================================================================
// Vulkan Surface Wrapper
//
// Provides a RAII wrapper for a vk::SurfaceKHR, which represents the abstract
// native window surface that Vulkan can render to. This class handles the
// creation and destruction of the surface, linking the Vulkan instance with
// the windowing system (GLFW).
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - RAII-style management of the vk::SurfaceKHR.
// - Bridges the gap between the Vulkan instance and the native window.
// - Non-copyable to ensure unique ownership.
// 
// Changelog:
// - 2026.01: Initial version / start of version history
//==============================================================================

#pragma once

#include "Helios/Platform/Renderer/Vulkan/VKInstance.h"
#include "Helios/Engine/Renderer/Window.h"

namespace Helios::Engine::Renderer::Vulkan {


	class VKSurface
	{
	public:
		VKSurface(const VKInstance& instance, Window& window);
		~VKSurface();

		// Prevent copying and moving
		VKSurface(const VKSurface&) = delete;
		VKSurface& operator=(const VKSurface&) = delete;
		VKSurface(VKSurface&&) = delete;
		VKSurface& operator=(VKSurface&&) = delete;

		// Accessor for the underlying Vulkan surface
		const vk::SurfaceKHR& Get() const { return m_surface; }
		operator const vk::SurfaceKHR&() const { return m_surface; }

	private:
		vk::SurfaceKHR m_surface = nullptr;
		// Store a reference to the instance for destruction
		const VKInstance& m_instance;
	};


} // namespace Helios::Engine::Renderer::Vulkan
