-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "Helios-Networking"
	architecture  "x86_64"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	kind          "StaticLib"

	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	pchheader "pch.h"
	pchsource "source/pch.cpp"

	includedirs {
		"source",
		"../../shared",
	}

	-- Libraries
--	VendorEnTT{}
--	VendorGlfw{}
--	VendorGlm{}
--	VendorSpdlog{}

	files {
		-- precompiled header
		"source/pch.h",
		"source/pch.cpp",
		-- project itself
		"source/Helios/Networking/**.h",
		"source/Helios/Networking/**.cpp",
	}

	filter "platforms:Windows"

		files {
			"source/Platform/System/Windows/**.h",
			"source/Platform/System/Windows/**.cpp",
		}

		defines {
		}

	filter "platforms:Linux"

		files {
			"source/Platform/System/Linux/**.h",
			"source/Platform/System/Linux/**.cpp",
		}

	filter "platforms:MacOS"

		files {
			"source/Platform/System/MacOS/**.h",
			"source/Platform/System/MacOS/**.cpp",
		}

	filter {}
