-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "Helios-Engine"
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
		"source/",
		"../../shared/",
	}

	-- Libraries
	VendorSpdlog{}
	VendorGlfw{}
--	VendorEnTT{}
	VendorGlm{}
		VendorVulkan{}
	VendorNVRHI()

	files {
		-- precompiled header
		"source/pch.h",
		"source/pch.cpp",
		-- project itself
		"source/Helios/Engine/**.h",
		"source/Helios/Engine/**.cpp",
		-- assets
--		"assets/**.vert",
--		"assets/**.frag",
	}
		
	filter "platforms:Windows"

		files {
			"source/Platform/System/Windows/**.h",
			"source/Platform/System/Windows/**.cpp",
		}

		defines {
			"HE_RENDERER_VULKAN",
			"HE_RENDERER_DX11",
			"HE_RENDERER_DX12",
		}

		files {
			"source/Platform/Renderer/DirectX/**.h",
			"source/Platform/Renderer/DirectX/**.cpp",
			"source/Platform/Renderer/Vulkan/**.h",
			"source/Platform/Renderer/Vulkan/**.cpp",
			"source/Platform/Renderer/OpenGL/**.h",
			"source/Platform/Renderer/OpenGL/**.cpp",
		}

	filter "platforms:Linux"

		files {
			"source/Platform/System/Linux/**.h",
			"source/Platform/System/Linux/**.cpp",
		}
		
		defines {
			"HE_RENDERER_VULKAN",
		}

		files {
			"source/Platform/Renderer/Vulkan/**.h",
			"source/Platform/Renderer/Vulkan/**.cpp",
			"source/Platform/Renderer/OpenGL/**.h",
			"source/Platform/Renderer/OpenGL/**.cpp",
		}

	filter "platforms:MacOS"

		files {
			"source/Platform/System/MacOS/**.h",
			"source/Platform/System/MacOS/**.cpp",
		}
		
		defines {
			"HE_RENDERER_VULKAN",
		}

		files {
			"source/Platform/Renderer/Metal/**.h",
			"source/Platform/Renderer/Metal/**.cpp",
			"source/Platform/Renderer/Vulkan/**.h",
			"source/Platform/Renderer/Vulkan/**.cpp",
			"source/Platform/Renderer/OpenGL/**.h",
			"source/Platform/Renderer/OpenGL/**.cpp",
		}

	filter {}
