#include "pch.h"
#include "RendererAPI.h"

#include "Helios/Engine/Core/Log.h"

namespace Helios::Engine
{

	void RendererAPI::SetAPI(API api)
	{
		bool supported = false;
		switch (api)
		{
		case API::Vulkan:
#			ifdef HE_RENDERER_VULKAN
				supported = true;
#			endif
			break;

		case API::OpenGL:
#			ifdef HE_RENDERER_OPENGL
				supported = true;
#			endif
			break;

		case API::DirectX12:
#			ifdef HE_RENDERER_DX12
				supported = true;
#			endif
			break;

		case API::Metal:
#			ifdef HE_RENDERER_METAL
				supported = true;
#			endif
			break;

		default:
			break;
		}

		if (supported)
		{
			s_API = api;
			LOG_RENDER_INFO("Set RendererAPI to {}", GetAPIName(api));
		}
		else
		{
			LOG_RENDER_EXCEPT("The requested RendererAPI::{} is not supported!", GetAPIName(api));
		}
	}

	const char* RendererAPI::GetAPIName(RendererAPI::API api)
	{
		switch (api)
		{
		case RendererAPI::API::Vulkan:    return "Vulkan";
		case RendererAPI::API::OpenGL:    return "OpenGL";
		case RendererAPI::API::DirectX12: return "DirectX12";
		case RendererAPI::API::Metal:     return "Metal";
		default:                          return "Unknown";
		}
	}

} // namespace Helios::Engine
