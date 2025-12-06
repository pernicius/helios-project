#include "pch.h"
#include "Helios/Engine/Renderer/DeviceManager.h"

#include "Helios/Engine/Renderer/RendererAPI.h"

#ifdef HE_RENDERER_OPENGL
#	include "Platform/Renderer/OpenGL/GLDeviceManager.h"
#endif
#ifdef HE_RENDERER_VULKAN
#	include "Platform/Renderer/Vulkan/VKDeviceManager.h"
#endif
#ifdef HE_RENDERER_DIRECTX
#	include "Platform/Renderer/DirectX/DX12DeviceManager.h"
#endif
#ifdef HE_RENDERER_METAL
#	include "Platform/Renderer/Metal/MTDeviceManager.h"
#endif

namespace Helios::Engine::Renderer {


	Scope<DeviceManager> DeviceManager::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			LOG_RENDER_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;

#		ifdef HE_RENDERER_OPENGL
		case RendererAPI::API::OpenGL: return CreateScope<GLDeviceManager>();
#		endif
#		ifdef HE_RENDERER_VULKAN
		case RendererAPI::API::Vulkan: return CreateScope<VKDeviceManager>();
#		endif
#		ifdef HE_RENDERER_DIRECTX
		case RendererAPI::API::DirectX: return CreateScope<DXDeviceManager>();
#		endif
#		ifdef HE_RENDERER_METAL
		case RendererAPI::API::Metal: return CreateScope<MTDeviceManager>();
#		endif
		}

		LOG_RENDER_ERROR("Unknown RendererAPI!");
		return nullptr;
	}


} //namespace Helios::Engine::Renderer
