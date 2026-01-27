//==============================================================================
// Renderer API Abstraction (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file Renderer.h
//==============================================================================
#include "pch.h"
#include "Helios/Engine/Renderer/Renderer.h"
#include "Helios/Engine/Renderer/RendererAPI.h"

#ifdef HE_RENDERER_VULKAN
#	include "Helios/Platform/Renderer/Vulkan/VKRenderer.h"
#endif
#ifdef HE_RENDERER_DIRECTX
#	include "Helios/Platform/Renderer/DirectX/DXRenderer.h"
#endif
#ifdef HE_RENDERER_METAL
#	include "Helios/Platform/Renderer/Metal/MTRenderer.h"
#endif

namespace Helios::Engine::Renderer {


	Scope<Renderer> Renderer::Create()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:
				LOG_RENDER_EXCEPT("RendererAPI::None is not supported!");
				return nullptr;

#		ifdef HE_RENDERER_VULKAN
			case RendererAPI::API::Vulkan:  return CreateScope<Vulkan::VKRenderer>();
#		endif
#		ifdef HE_RENDERER_DIRECTX
			case RendererAPI::API::DirectX: return CreateScope<DirectX::DXRenderer>();
#		endif
#		ifdef HE_RENDERER_METAL
			case RendererAPI::API::Metal:   return CreateScope<Metal::MTRenderer>();
#		endif
		}

		LOG_RENDER_EXCEPT("Unknown RendererAPI!");
		return nullptr;
	}


} // namespace Helios::Engine::Renderer
