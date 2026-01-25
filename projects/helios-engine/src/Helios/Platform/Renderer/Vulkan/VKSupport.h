//==============================================================================
// Vulkan API Support Detection
//
// Provides platform-specific detection of Vulkan API availability. Checks for
// required Vulkan runtime components, driver support, and API version
// compatibility. Used by the RendererAPI system to determine if Vulkan can be
// selected as the active graphics backend on the current platform.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

namespace Helios::Engine::Renderer::Vulkan {


	bool IsAPISupported();


} // namespace Helios::Engine::Renderer::Vulkan
