//==============================================================================
// Renderer API Abstraction
//
// Provides platform-agnostic renderer API selection and management. Supports
// multiple graphics APIs (DirectX, Metal, Vulkan) with runtime detection of
// platform capabilities. Maintains a global API state and provides named
// enumeration for API types. Used by the renderer subsystem to abstract
// graphics backend selection.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

namespace Helios::Engine::Renderer {


	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,

			DirectX,
			Metal,
			Vulkan,

			_max
		};
		static constexpr std::array<std::string_view, static_cast<size_t>(API::_max)> API_NAMES = {
			"None",

			"DirectX",
			"Metal",
			"Vulkan",
		};

		// Check if the given RendererAPI::API is supported on this platform
		static bool IsAPISupported(API api);

		// Check which RendererAPIs are supported and set the RendererAPI to the first one found
		static void CheckAPISupport();

		// Get the current RendererAPI::API
		static inline API GetAPI() { return s_API; }

		// Set the RendererAPI::API to the given one if supported
		static bool SetAPI(API api);
		
		// Get the name of the given RendererAPI::API
		static std::string_view GetAPIName(API api);

	private:
		static inline API s_API = API::None;
	};


}// namespace Helios::Engine::Renderer
