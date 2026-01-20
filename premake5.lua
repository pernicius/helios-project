printf("Premake version: '%s'", _PREMAKE_VERSION)
printf("Premake target:  '%s'", _TARGET_OS)

---------------------------------
-- [ WORKSPACE CONFIGURATION ] --
---------------------------------
workspace "Helios-Project"

	architecture "x86_64"

	configurations {
		"Debug",
		"Release"
		-- TODO: do we need more? like "Relese-Candidate", "Alpha", "Beta"....
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

	filter {}

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
		fatalwarnings "All"

		optimize "Off"

	filter "configurations:Release"
		defines {
			"BUILD_RELEASE",
			"NDEBUG",
		}
		symbols  "Off"
		runtime  "Release"
		fatalwarnings "All"

		optimize "Off"
--		optimize "On"
--		optimize "Speed"
--		optimize "Full"

	filter {}

	-------------------------------
	-- [ PROJECT CONFIGURATION ] --
	-------------------------------
	dir_bin    = (path.join("%{wks.location}", "_bin") .. "/")
	dir_build  = (path.join("%{wks.location}", "_build") .. "/")
	dir_config = ("%{string.lower(cfg.platform)}-%{string.lower(cfg.buildcfg)}" .. "/")

	include("premake5_misc.lua")
	group "Vendor"
		dir_group = ("vendor" .. "/")
		include("vendor/dev-tools/premake/")
		include("vendor/header/")
		include("vendor/libs/")
	group "Helios"
		dir_group = ("helios" .. "/")
		include("projects/helios-engine/")
		include("projects/apps/sandbox/")
	group "Apps"
		dir_group = ("apps" .. "/")
		include("projects/apps/template/")
	group ""
