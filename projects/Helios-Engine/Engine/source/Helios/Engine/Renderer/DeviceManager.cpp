#include "pch.h"
#include "DeviceManager.h"

namespace Helios::Engine {


	Renderer::Specification DeviceManager::s_Spec{};
	nvrhi::GraphicsAPI s_api;

	Ref<Window> DeviceManager::s_MainWindow = nullptr;


	std::vector<nvrhi::GraphicsAPI> DeviceManager::GetSupportedAPI()
	{
		std::vector<nvrhi::GraphicsAPI> result;

#		ifdef HE_RENDERER_VULKAN
			result.push_back(nvrhi::GraphicsAPI::VULKAN);
#		endif
#		ifdef HE_RENDERER_DX11
			result.push_back(nvrhi::GraphicsAPI::D3D11);
#		endif
#		ifdef HE_RENDERER_DX12
			result.push_back(nvrhi::GraphicsAPI::D3D12);
#		endif

		return result;
	}


	bool DeviceManager::CheckAPISupport(const nvrhi::GraphicsAPI api)
	{
#		ifdef HE_RENDERER_VULKAN
			if (api == nvrhi::GraphicsAPI::VULKAN)
				return true;
#		endif
#		ifdef HE_RENDERER_DX11
			if (api == nvrhi::GraphicsAPI::D3D11)
				return true;
#		endif
#		ifdef HE_RENDERER_DX12
			if (api == nvrhi::GraphicsAPI::D3D12)
				return true;
#		endif

		return false;
	}


	Ref<Window> DeviceManager::CreateMainWindow()
	{
		s_MainWindow = Window::Create(s_Spec);
		return s_MainWindow;
	}


} // end of namespace Helios::Engine
