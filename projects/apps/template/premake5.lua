-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "Template"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"

	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	pchheader "pch.h"
	pchsource "pch.cpp"

	includedirs {
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

		kind "ConsoleApp"

		defines {
		}
		
		debugargs {
		}

	filter "configurations:Release"

		kind "WindowedApp"

		defines {
		}

	filter {}
