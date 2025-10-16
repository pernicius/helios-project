-------------------------
-- [DEPENDANCY HELPER] --
-------------------------
function Lib_HeliosEngine()
	includedirs "%{wks.location}/projects/engine/engine/src/"

	filter "kind:*App"
		links "Helios-Engine"
	filter {}

	Lib_HeliosShared{}

	-- vendor/header
	Lib_Spdlog{}

	-- vendor/lib
	Lib_GLFW{}
end


----------------------
-- [PROJECT CONFIG] --
----------------------
project "Helios-Engine"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	kind          "StaticLib"

	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	pchheader "pch.h"
	pchsource "pch.cpp"

	includedirs {
		"%{prj.location}",
		"src/",
	}

	files {
		"pch.*",
		"src/Helios/**.h",
		"src/Helios/**.c",
		"src/Helios/**.hpp",
		"src/Helios/**.cpp",
	}

	filter "platforms:Windows"

		files {
			"src/Platform/System/Windows/**.h",
			"src/Platform/System/Windows/**.cpp",
		}

		defines {
			"HE_RENDERER_VULKAN", -- should be supported on all platforms
			"HE_RENDERER_OPENGL", -- should be supported on all platforms, works as a fallback for vulkan
--			"HE_RENDERER_DX12",   -- supported only on platforms:windows
--			"HE_RENDERER_DX11",   -- supported only on platforms:windows, works as a fallback for dx12
		}

		files {
			"src/Platform/Renderer/Vulkan/**.h",
			"src/Platform/Renderer/Vulkan/**.cpp",
		}

	filter "platforms:Linux"

		files {
			"src/Platform/System/Linux/**.h",
			"src/Platform/System/Linux/**.cpp",
		}

		defines {
			"HE_RENDERER_VULKAN", -- should be supported on all platforms
			"HE_RENDERER_OPENGL", -- should be supported on all platforms, works as a fallback for vulkan
		}

		files {
			"src/Platform/Renderer/Vulkan/**.h",
			"src/Platform/Renderer/Vulkan/**.cpp",
		}

	filter "platforms:MacOS"

		files {
			"src/Platform/System/MacOS/**.h",
			"src/Platform/System/MacOS/**.cpp",
		}

		defines {
			"HE_RENDERER_VULKAN", -- should be supported on all platforms
			"HE_RENDERER_OPENGL", -- should be supported on all platforms, works as a fallback for vulkan
--			"HE_RENDERER_METAL",  -- supported only on platforms:MacOS
		}

		files {
			"src/Platform/Renderer/Vulkan/**.h",
			"src/Platform/Renderer/Vulkan/**.cpp",
		}

	filter {}

	-- Dependencies
	Lib_HeliosEngine{}
	
	filter "configurations:Debug"

		defines {
		}

	filter "configurations:Release"

		defines {
		}
		
	filter {}
