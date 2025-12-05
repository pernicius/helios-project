#include "pch.h"
#include "Helios/Engine/Renderer/Context.h"

#include "Helios/Engine/Renderer/RendererAPI.h"

#ifdef HE_RENDERER_OPENGL
#	include "Platform/Renderer/OpenGL/GLContext.h"
#endif
#ifdef HE_RENDERER_VULKAN
#	include "Platform/Renderer/Vulkan/VKContext.h"
#endif
#ifdef HE_RENDERER_DIRECTX
#	include "Platform/Renderer/DirectX/DX12Context.h"
#endif
#ifdef HE_RENDERER_METAL
#	include "Platform/Renderer/Metal/MTContext.h"
#endif

namespace Helios::Engine::Renderer {


	Scope<Context> Context::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			LOG_RENDER_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;

#		ifdef HE_RENDERER_OPENGL
			case RendererAPI::API::OpenGL: return CreateScope<GLContext>();
#		endif
#		ifdef HE_RENDERER_VULKAN
			case RendererAPI::API::Vulkan: return CreateScope<VKContext>();
#		endif
#		ifdef HE_RENDERER_DIRECTX
			case RendererAPI::API::DirectX: return CreateScope<DXContext>();
#		endif
#		ifdef HE_RENDERER_METAL
			case RendererAPI::API::Metal: return CreateScope<MTContext>();
#		endif
		}

		LOG_RENDER_ERROR("Unknown RendererAPI!");
		return nullptr;
	}


} //namespace Helios::Engine::Renderer
