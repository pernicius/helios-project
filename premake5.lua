---------------------------------
-- [ WORKSPACE CONFIGURATION ] --
---------------------------------
workspace "Helios-Project"

	architecture "x86_64"

	configurations {
		"Debug",
		"Release"
	}

	platforms {
		"Windows",
		"Linux",
		"MacOS"
	}
	
	defaultplatform "Windows"
	startproject "Sandbox"

	--------------------------------
	-- [ PLATFORM CONFIGURATION ] --
	--------------------------------
	filter "platforms:Windows"
		system  "windows"
	filter "platforms:Linux"
		system  "linux"
	filter "platforms:MacOS"
		system  "macosx"

	-- Windows/MSVC only
	filter { "system:windows", "action:vs*" }
		flags { "MultiProcessorCompile", "NoMinimalRebuild" }
		systemversion "latest"
		-- required by spdlog
		buildoptions "/utf-8"

	-------------------------------------
	-- [ DEBUG/RELEASE CONFIGURATION ] --
	-------------------------------------
	filter "configurations:Debug"
		defines {
			"BUILD_DEBUG",
			"_DEBUG",
		}
		symbols  "On"
		runtime  "Debug"
		optimize "Off"

	filter "configurations:Release"
		defines {
			"BUILD_RELEASE",
			"NDEBUG",
		}
		symbols  "Off"
		runtime  "Release"
		
		optimize "Off"
--		optimize "On"
--		optimize "Speed"
--		optimize "Full"

	filter {}

	-------------------------------
	-- [ PROJECT CONFIGURATION ] --
	-------------------------------

	dir_bin     = "%{wks.location}/_bin/"
	dir_build   = "%{wks.location}/_build/"
	dir_config  = "%{string.lower(cfg.platform)}-%{string.lower(cfg.buildcfg)}/"

	group "Vendor"
		dir_group = "Helios-Vendor/"
		include("vendor/dev-tools/premake/")

	group "Helios/Engine"
		dir_group = "Helios-Engine/"
		include("projects/engine/")
		include("projects/apps/sandbox/")

	group "Helios/Apps"
		dir_group = "Helios-Apps/"
		include("projects/apps/template/")

	group ""
