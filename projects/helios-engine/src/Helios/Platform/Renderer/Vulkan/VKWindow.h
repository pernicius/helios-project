//==============================================================================
// Vulkan Window Implementation
//
// Provides a Vulkan-specific window implementation derived from the base
// Window class. Configures GLFW window hints for Vulkan compatibility
// (including disabling OpenGL context creation). Manages Vulkan surface
// creation and integration with the GLFW windowing system. Non-copyable to
// ensure unique ownership of window resources.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Reworked to use ConfigManager insead of IniParser directly
// - 2026.01: Initial version / start of version history
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
