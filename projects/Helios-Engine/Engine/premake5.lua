-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "Engine"
	architecture  "x86_64"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	kind          "StaticLib"

	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

--	pchheader "pch.h"
--	pchsource "source/pch.cpp"

	-- Libraries
--	VendorEnTT{}
--	VendorGlfw{}
--	VendorGlm{}
--	VendorSpdlog{}


	includedirs {
		"source",
		"../../shared",
	}


	files {
		-- precompiled header
--		"source/pch.h",
--		"source/pch.cpp",
		-- project itself
		"source/Helios/Engine/**.h",
		"source/Helios/Engine/**.cpp",
		-- assets
		"assets/**.vert",
		"assets/**.frag",
	}


	filter "platforms:Windows"

		files {
			"source/Platform/System/Windows/**.h",
			"source/Platform/System/Windows/**.cpp",
		}

		defines {
--			"BUILDWITH_RENDERER_DIRECTX",
			"BUILDWITH_RENDERER_VULKAN",
--			"BUILDWITH_RENDERER_OPENGL",
		}

		files {
			"source/Platform/Renderer/DirectX/**.h",
			"source/Platform/Renderer/DirectX/**.cpp",
			"source/Platform/Renderer/Vulkan/**.h",
			"source/Platform/Renderer/Vulkan/**.cpp",
			"source/Platform/Renderer/OpenGL/**.h",
			"source/Platform/Renderer/OpenGL/**.cpp",
		}

--		VendorDirectX{}
--		VendorVulkan{}
--		VendorOpenGL{}



	filter "platforms:Linux"

		files {
			"source/Platform/System/Linux/**.h",
			"source/Platform/System/Linux/**.cpp",
		}
		
		defines {
			"BUILDWITH_RENDERER_VULKAN",
			"BUILDWITH_RENDERER_OPENGL",
		}

		files {
			"source/Platform/Renderer/Vulkan/**.h",
			"source/Platform/Renderer/Vulkan/**.cpp",
			"source/Platform/Renderer/OpenGL/**.h",
			"source/Platform/Renderer/OpenGL/**.cpp",
		}

--		VendorVulkan{}
--		VendorOpenGL{}


	filter "platforms:MacOS"

		files {
			"source/Platform/System/MacOS/**.h",
			"source/Platform/System/MacOS/**.cpp",
		}
		
		defines {
			"BUILDWITH_RENDERER_METAL",
			"BUILDWITH_RENDERER_VULKAN",
			"BUILDWITH_RENDERER_OPENGL",
		}

		files {
			"source/Platform/Renderer/Metal/**.h",
			"source/Platform/Renderer/Metal/**.cpp",
			"source/Platform/Renderer/Vulkan/**.h",
			"source/Platform/Renderer/Vulkan/**.cpp",
			"source/Platform/Renderer/OpenGL/**.h",
			"source/Platform/Renderer/OpenGL/**.cpp",
		}

--		VendorMetal{}
--		VendorVulkan{}
--		VendorOpenGL{}


	filter {}


--	prebuildmessage "Updating version information..."
--	prebuildcommands {
--		"\"%{wks.location}Tools/build_inc/_bin/build_inc_" .. os.host() .. "\" -bfile \"%{prj.location}Source/Config/Version.h\" -bdef VERSION_BUILD"
--	}


--	postbuildmessage "Copying assets to the target folder..."
--	postbuildcommands {
--		"{COPYDIR} \"%{prj.location}assets\" \"" .. dir_bin .. dir_group .. dir_config .. "Assets\""
--	}