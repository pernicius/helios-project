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
