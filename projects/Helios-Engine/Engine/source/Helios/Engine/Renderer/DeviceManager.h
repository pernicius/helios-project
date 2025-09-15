#pragma once

#include "Helios/Engine/Renderer/Renderer.h"
#include "Helios/Engine/Renderer/Window.h"

namespace Helios::Engine {


	class DeviceManager
	{
	public:
		static std::vector<nvrhi::GraphicsAPI> GetSupportedAPI();
		static bool CheckAPISupport(const nvrhi::GraphicsAPI api);
		static nvrhi::GraphicsAPI GetAPI() { return s_api; }

		static void SetSpecification(Renderer::Specification spec) { s_Spec = spec; }
		static Renderer::Specification& GetSpecification() { return s_Spec; }

		static Ref<Window> CreateMainWindow();
		static Ref<Window> GetMainWindow() { return s_MainWindow; }

	private:
		DeviceManager() = delete;
		~DeviceManager() = delete;

//		bool CreateRenderer_VK();
//		bool CreateRenderer_DX11();
//		bool CreateRenderer_DX12();

		static Renderer::Specification s_Spec;
		static nvrhi::GraphicsAPI s_api;

		static Ref<Window> s_MainWindow;
	};


} // end of namespace Helios::Engine
