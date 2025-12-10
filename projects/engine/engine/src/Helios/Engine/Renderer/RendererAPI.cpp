#include "pch.h"
#include "Helios/Engine/Renderer/RendererAPI.h"

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

namespace Helios::Engine::Renderer
{

	bool RendererAPI::SetAPI(API api)
	{
		bool supported = false;
		switch (api)
		{
#		ifdef HE_RENDERER_OPENGL
			case API::OpenGL:  supported = OpenGL::IsAPISupported();    break;
#		endif
#		ifdef HE_RENDERER_VULKAN
			case API::Vulkan:  supported = Vulkan::IsAPISupported();    break;
#		endif
#		ifdef HE_RENDERER_DIRECTX
			case API::DirectX: supported = DirectX::IsAPISupported(); break;
#		endif
#		ifdef HE_RENDERER_METAL
			case API::Metal:   supported = Metal::IsAPISupported();     break;
#		endif
		default: break;
		}

		if (supported) {
			s_API = api;
			LOG_RENDER_INFO("Selected RendererAPI: {}", GetAPIName(api));
		}
		else {
			LOG_RENDER_EXCEPT("The requested RendererAPI::{} is not supported!", GetAPIName(api));
		}

		return supported;
	}

	std::string_view RendererAPI::GetAPIName(RendererAPI::API api)
	{
		size_t idx = static_cast<size_t>(api);
		if (idx < API_NAMES.size())
			return API_NAMES[idx];
		return "Unknown";
	}


	bool RendererAPI::IsAPISupported(API api)
	{
		switch (api)
		{
#		ifdef HE_RENDERER_OPENGL
			case RendererAPI::API::OpenGL:  return OpenGL::IsAPISupported();
#		endif
#		ifdef HE_RENDERER_VULKAN
			case RendererAPI::API::Vulkan:  return Vulkan::IsAPISupported();
#		endif
#		ifdef HE_RENDERER_DIRECTX
			case RendererAPI::API::DirectX: return DirectX::IsAPISupported();
#		endif
#		ifdef HE_RENDERER_METAL
			case RendererAPI::API::Metal:   return Metal::IsAPISupported();
#		endif
			default:                        return false;
		}
	}

	void RendererAPI::CheckAPISupport()
	{
		bool isSet = false;
		for (int i = 1; i < static_cast<int>(RendererAPI::API::_max); ++i) {
			RendererAPI::API api = static_cast<RendererAPI::API>(i);
			if (IsAPISupported(api) and (isSet == false)) {
				isSet = SetAPI(api);
			}
		}
	}

} // namespace Helios::Engine::Renderer
