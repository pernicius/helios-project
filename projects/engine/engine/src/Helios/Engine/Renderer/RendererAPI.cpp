#include "pch.h"
#include "Helios/Engine/Renderer/RendererAPI.h"

#include "Helios/Engine/Core/Log.h"

#ifdef HE_RENDERER_VULKAN
#	include "Platform/Renderer/Vulkan/VKSupport.h"
#endif
#ifdef HE_RENDERER_OPENGL
#	include "Platform/Renderer/OpenGL/GLSupport.h"
#endif
#ifdef HE_RENDERER_DIRECTX
#	include "Platform/Renderer/DirectX/DXSupport.h"
#endif
#ifdef HE_RENDERER_METAL
#	include "Platform/Renderer/Metal/MTSupport.h"
#endif

namespace Helios::Engine
{

	bool RendererAPI::Set(API api)
	{
		bool supported = false;
		switch (api)
		{
#		ifdef HE_RENDERER_OPENGL
			case API::OpenGL:  supported = OpenGL::IsSupported();    break;
#		endif
#		ifdef HE_RENDERER_VULKAN
			case API::Vulkan:  supported = Vulkan::IsSupported();    break;
#		endif
#		ifdef HE_RENDERER_DIRECTX
			case API::DirectX: supported = DirectX::IsSupported(); break;
#		endif
#		ifdef HE_RENDERER_METAL
			case API::Metal:   supported = Metal::IsSupported();     break;
#		endif
		default: break;
		}

		if (supported) {
			s_API = api;
			LOG_RENDER_INFO("Set RendererAPI to {}", GetName(api));
		}
		else {
			LOG_RENDER_EXCEPT("The requested RendererAPI::{} is not supported!", GetName(api));
		}

		return supported;
	}

	std::string_view RendererAPI::GetName(RendererAPI::API api)
	{
		size_t idx = static_cast<size_t>(api);
		if (idx < API_NAMES.size())
			return API_NAMES[idx];
		return "Unknown";
	}


	bool RendererAPI::IsSupported(API api)
	{
		switch (api)
		{
#		ifdef HE_RENDERER_OPENGL
			case RendererAPI::API::OpenGL:  return OpenGL::IsSupported();
#		endif
#		ifdef HE_RENDERER_VULKAN
			case RendererAPI::API::Vulkan:  return Vulkan::IsSupported();
#		endif
#		ifdef HE_RENDERER_DX12
			case RendererAPI::API::DirectX: return DirectX::IsSupported();
#		endif
#		ifdef HE_RENDERER_METAL
			case RendererAPI::API::Metal:   return Metal::IsSupported();
#		endif
			default:                        return false;
		}
	}

	void RendererAPI::CheckSupport()
	{
		bool isSet = false;
		for (int i = 1; i < static_cast<int>(RendererAPI::API::_max); ++i) {
			RendererAPI::API api = static_cast<RendererAPI::API>(i);
			if (IsSupported(api) and (isSet == false)) {
				isSet = Set(api);
			}
		}
	}

} // namespace Helios::Engine
