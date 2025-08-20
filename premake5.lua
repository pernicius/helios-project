---------------------------------
-- [ WORKSPACE CONFIGURATION ] --
---------------------------------
workspace "Helios-Project"

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
	
	startproject "Engine"


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
	-- [ LIBRARIES (UTIL-FUNC) ] --
	-------------------------------
	
	include("premake5_libs.lua")


	-------------------------------
	-- [ PROJECT CONFIGURATION ] --
	-------------------------------

	dir_bin     = "%{wks.location}/_bin/"
	dir_build   = "%{wks.location}/_build/"
	dir_config  = "%{string.lower(cfg.platform)}-%{string.lower(cfg.buildcfg)}/"
	dir_project = "%{string.lower(prj.name)}"

	group "Helios-Engine"
		dir_group = "Helios-Engine/"
		include("projects/Helios-Engine/")

	group "Helios-App"
		dir_group = "Helios-App/"
		include("projects/Helios-App/")

	group "Shared"
		dir_group = "Shared"
		include("projects/shared/")

	group "Vendor"
		dir_group = "Vendor/"
		include("vendor/")

	group "Misc"
		dir_group = "Misc/"
		include("docu/")
--		include("Tools/")

	group ""