------------------
-- [ Renderer ] --
------------------


-- [ VULKAN ]

function Src_RendererVulkan()
	if os.getenv("VULKAN_SDK") then
		files {
			"src/Helios/Platform/Renderer/Vulkan/**.h",
			"src/Helios/Platform/Renderer/Vulkan/**.cpp",
		}
	end
end

function Lib_RendererVulkan()
	if os.getenv("VULKAN_SDK") then

		defines { "HE_RENDERER_VULKAN" }
		includedirs "%VULKAN_SDK%/Include"
		
		filter "kind:*App"
			libdirs "%VULKAN_SDK%/Lib"
			links "vulkan-1"
		filter "platforms:Windows"
			defines {
				-- TODO: Do we really need this?
				"VK_USE_PLATFORM_WIN32_KHR",
			}
		filter {}
	end
end


-- [ DIRECTX12 (Windows) ]

function Src_RendererDirectX12()
	filter "platforms:Windows"
		files {
			"src/Helios/Platform/Renderer/DirectX12/**.h",
			"src/Helios/Platform/Renderer/DirectX12/**.cpp",
		}
	filter {}
end

function Lib_RendererDirectX12()
	filter "platforms:Windows"
		defines { "HE_RENDERER_DIRECTX12" }
	filter "kind:*App, platforms:Windows"
		links { "d3d12", "dxgi", "d3dcompiler" }
	filter {}
end


-- [ METAL (MacOS) ]

function Src_RendererMetal()
	filter "platforms:MacOS"
		files {
			"src/Helios/Platform/Renderer/Metal/**.h",
			"src/Helios/Platform/Renderer/Metal/**.mm",
		}
	filter {}
end

function Lib_RendererMetal()
	filter "platforms:MacOS"
		defines { "HE_RENDERER_METAL" }
	filter "kind:*App, platforms:MacOS"
		links { "Metal.framework", "MetalKit.framework", "QuartzCore.framework" }
	filter {}
end
