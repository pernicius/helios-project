#include "pch.h"
#include "Helios/Engine/Renderer/Window.h"

#include "Helios/Engine/Core/Log.h"
#include "Helios/Engine/Renderer/RendererAPI.h"

#ifdef HE_RENDERER_OPENGL
#	include "Platform/Renderer/OpenGL/GLWindow.h"
#endif
#ifdef HE_RENDERER_VULKAN
#	include "Platform/Renderer/Vulkan/VKWindow.h"
#endif
#ifdef HE_RENDERER_DIRECTX
#	include "Platform/Renderer/DirectX/DX12Window.h"
#endif
#ifdef HE_RENDERER_METAL
#	include "Platform/Renderer/Metal/MTWindow.h"
#endif

namespace Helios::Engine {


	Scope<Window> Window::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			LOG_RENDER_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;

#		ifdef HE_RENDERER_OPENGL
			case RendererAPI::API::OpenGL: return CreateScope<GLWindow>();
#		endif
#		ifdef HE_RENDERER_VULKAN
			case RendererAPI::API::Vulkan: return CreateScope<VKWindow>();
#		endif
#		ifdef HE_RENDERER_DIRECTX
			case RendererAPI::API::DirectX: return CreateScope<DXWindow>();
#		endif
#		ifdef HE_RENDERER_METAL
			case RendererAPI::API::Metal: return CreateScope<MTWindow>();
#		endif
		}

		LOG_RENDER_ERROR("Unknown RendererAPI!");
		return nullptr;
	}


}// namespace Helios::Engine
