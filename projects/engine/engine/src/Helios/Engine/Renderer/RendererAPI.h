#pragma once

#include <array>
#include <string_view>

namespace Helios::Engine {


	class RendererAPI
	{
	public:
		/**
		 * @brief The supported Renderer APIs
		 */
		enum class API
		{
			None = 0,

			DirectX,
			Metal,
			Vulkan,
			OpenGL,

			_max
		};
		static constexpr std::array<std::string_view, static_cast<size_t>(API::_max)> API_NAMES = {
			"None",

			"DirectX",
			"Metal",
			"Vulkan",
			"OpenGL",
		};

		/**
		 * @brief Check if the given RendererAPI::API is supported on this platform
		 * @param api The RendererAPI::API to check
		 * @return bool True if supported, false otherwise
		 */
		static bool IsAPISupported(API api);

		/**
		 * @brief Check which RendererAPIs are supported and set the RendererAPI to the first one found
		 */
		static void CheckAPISupport();

		/**
		 * @brief Get the current RendererAPI::API
		 * @return API The current RendererAPI::API
		 */
		static inline API GetAPI() { return s_API; }

		/**
		 * @brief Set the RendererAPI::API to the given one if supported
		 * @param api The RendererAPI::API to set
		 * @return bool True if the RendererAPI::API was set, false otherwise
		 */
		static bool SetAPI(API api);
		
		/**
		 * @brief Get the name of the given RendererAPI::API
		 * @param api The RendererAPI::API to get the name of
		 * @return std::string_view The name of the given RendererAPI::API
		 */
		static std::string_view GetAPIName(API api);

	private:
		static inline API s_API = API::None;
	};


}// namespace Helios::Engine
