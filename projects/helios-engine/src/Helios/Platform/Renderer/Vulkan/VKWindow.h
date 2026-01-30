//==============================================================================
// Vulkan Window Implementation
//
// Provides a Vulkan-specific window implementation derived from the base
// Window class. Its primary responsibility is to configure GLFW window hints
// for Vulkan compatibility (e.g., disabling the OpenGL context) before the
// window is created and initialized.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - Implements the abstract `Window` interface for a Vulkan context.
// - Configures GLFW to be Vulkan-ready by setting appropriate window hints.
// - Ensures unique ownership of window resources by being non-copyable.
//
// Changelog:
// - 2026.01: Fixed window name/title
// - 2026.01: Changed to the new config system
// - 2026.01: Reworked to use ConfigManager instead of IniParser directly
// - 2026.01: Initial version
//==============================================================================
#pragma once

#include "Helios/Engine/Renderer/Window.h"

namespace Helios::Engine::Renderer::Vulkan
{


	class VKWindow : public Window
	{
	public:
		VKWindow(const std::string name);
		~VKWindow();

		// Non-copyable
		VKWindow(const VKWindow&) = delete;
		VKWindow& operator=(const VKWindow&) = delete;

	private:
		void ConfigureWindowHints();
	};


} // namespace Helios::Engine::Renderer::Vulkan
