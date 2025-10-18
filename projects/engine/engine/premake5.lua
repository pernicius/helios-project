-------------------------
-- [DEPENDANCY HELPER] --
-------------------------
include "premake5_renderer"

function Lib_HeliosEngine()
	includedirs "%{wks.location}/projects/engine/engine/src/"

	filter "kind:*App"
		links "Helios-Engine"
	filter {}

	Lib_HeliosShared{}

	-- Renderer
	Lib_RendererOpenGL{}
	Lib_RendererVulkan{}

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
		"premake5.lua",
		"premake5_renderer.lua",
		"pch.*",
		"src/Helios/**.h",
		"src/Helios/**.c",
		"src/Helios/**.hpp",
		"src/Helios/**.cpp",
	}
	Src_RendererOpenGL{}
	Src_RendererVulkan{}

	filter "platforms:Windows"

		files {
			"src/Platform/System/Windows/**.h",
			"src/Platform/System/Windows/**.cpp",
		}

	filter "platforms:Linux"

		files {
			"src/Platform/System/Linux/**.h",
			"src/Platform/System/Linux/**.cpp",
		}

	filter "platforms:MacOS"

		files {
			"src/Platform/System/MacOS/**.h",
			"src/Platform/System/MacOS/**.cpp",
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
