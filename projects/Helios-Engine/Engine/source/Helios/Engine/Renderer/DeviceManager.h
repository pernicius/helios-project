#pragma once

namespace Helios::Engine {


	class DeviceManager
	{
	public:

		enum class API {
			DirectX11,
			DirectX12,
			Vulkan,
		};

		static std::vector<API> GetSupportedAPI();
		static bool GetSupportedAPI(API api);

		API GetAPI() const { return m_api; };

		static bool Create(API api);

	private:
		bool Create_VK();
		bool Create_DX11();
		bool Create_DX12();

		API m_api;
	};


} // end of namespace Helios::Engine
