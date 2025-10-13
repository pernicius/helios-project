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
		"src/**.h",
		"src/**.c",
		"src/**.hpp",
		"src/**.cpp",
	}

	-- Dependencies
	Lib_HeliosEngine{}
	
	filter "configurations:Debug"

		defines {
		}

	filter "configurations:Release"

		defines {
		}
		
	filter {}
