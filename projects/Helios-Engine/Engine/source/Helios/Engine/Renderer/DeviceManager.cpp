#include "pch.h"
#include "DeviceManager.h"

namespace Helios::Engine {


	std::vector<DeviceManager::API> DeviceManager::GetSupportedAPI()
	{
		std::vector<API> result;

#		ifdef HE_RENDERER_VULKAN
			result.push_back(API::Vulkan);
#		endif
#		ifdef HE_RENDERER_DX11
			result.push_back(API::DirectX11);
#		endif
#		ifdef HE_RENDERER_DX12
			result.push_back(API::DirectX12);
#		endif

		return result;
	}


	bool DeviceManager::GetSupportedAPI(API api)
	{
#		ifdef HE_RENDERER_VULKAN
			if (api == API::Vulkan)
				return true;
#		endif
#		ifdef HE_RENDERER_DX11
			if (api == API::DirectX11)
				return true;
#		endif
#		ifdef HE_RENDERER_DX12
			if (api == API::DirectX12)
				return true;
#		endif

		return false;
	}


} // end of namespace Helios::Engine
