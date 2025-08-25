-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "nvrhi.vulkan"
	kind          "StaticLib"
	architecture  "x86_64"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	targetname    "nvrhi_vulkan"
	
	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	-- move project in the correct dir
	basedir("nvrhi")
	
	includedirs {
		"nvrhi/include",
		"nvrhi/src",
		-- Vulkan-Headers
		"nvrhi/thirdparty/Vulkan-Headers/include",
	}
	
	files {
		-- include
		"nvrhi/include/nvrhi/vulkan.h",
		-- src
		"nvrhi/src/common/versioning.h",
		"nvrhi/src/vulkan/vulkan-allocator.cpp",
		"nvrhi/src/vulkan/vulkan-buffer.cpp",
		"nvrhi/src/vulkan/vulkan-commandlist.cpp",
		"nvrhi/src/vulkan/vulkan-compute.cpp",
		"nvrhi/src/vulkan/vulkan-constants.cpp",
		"nvrhi/src/vulkan/vulkan-device.cpp",
		"nvrhi/src/vulkan/vulkan-graphics.cpp",
		"nvrhi/src/vulkan/vulkan-meshlets.cpp",
		"nvrhi/src/vulkan/vulkan-queries.cpp",
		"nvrhi/src/vulkan/vulkan-queue.cpp",
		"nvrhi/src/vulkan/vulkan-raytracing.cpp",
		"nvrhi/src/vulkan/vulkan-resource-bindings.cpp",
		"nvrhi/src/vulkan/vulkan-shader.cpp",
		"nvrhi/src/vulkan/vulkan-staging-texture.cpp",
		"nvrhi/src/vulkan/vulkan-state-tracking.cpp",
		"nvrhi/src/vulkan/vulkan-texture.cpp",
		"nvrhi/src/vulkan/vulkan-upload.cpp",
		"nvrhi/src/vulkan/vulkan-backend.h",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		files {
		}

		defines {
			"VK_USE_PLATFORM_WIN32_KHR",
			"NOMINMAX",
		}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"

		files {
		}

		defines {
		}

	filter "system:macosx"
		staticruntime "On"

		files {
		}

		defines {
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "On"
