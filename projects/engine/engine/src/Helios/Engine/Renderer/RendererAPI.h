#pragma once

namespace Helios::Engine {


	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,

			Vulkan,
			OpenGL,
			DirectX12,
			Metal,
		};

		static inline API GetAPI() { return s_API; }
		static void SetAPI(API api);

		static const char* GetAPIName(RendererAPI::API api);

	private:
		static inline API s_API = API::None;


	};


}// namespace Helios::Engine
