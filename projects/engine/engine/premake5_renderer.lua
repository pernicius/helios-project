--------------
-- [OpenGL] --
--------------

function Src_RendererOpenGL()
	files {
		"src/Platform/Renderer/OpenGL/**.h",
		"src/Platform/Renderer/OpenGL/**.cpp",
	}
end

function Lib_RendererOpenGL()
	defines { "HE_RENDERER_OPENGL" }
--	includedirs ""
		
	filter "kind:*App"
--		libdirs ""
--		links ""
	filter {}

	Lib_glad{}
end

--------------
-- [Vulkan] --
--------------

function Src_RendererVulkan()
	if os.getenv("VULKAN_SDK") then
		files {
			"src/Platform/Renderer/Vulkan/**.h",
			"src/Platform/Renderer/Vulkan/**.cpp",
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
		filter {}
	end
end
