-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "nvrhi.d3d11"
	kind          "StaticLib"
	architecture  "x86_64"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	targetname    "nvrhi_d3d11"
	
	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	-- move project in the correct dir
	basedir("nvrhi")
	
	links { "d3d11", "dxgi", "dxguid" }

	includedirs {
		"nvrhi/include",
		"nvrhi/src",
	}
	
	files {
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		files {
			-- include
			"nvrhi/include/nvrhi/d3d11.h",
			-- src
			"nvrhi/src/common/dxgi-format.h",
			"nvrhi/src/common/dxgi-format.cpp",
			"nvrhi/src/d3d11/d3d11-buffer.cpp",
			"nvrhi/src/d3d11/d3d11-commandlist.cpp",
			"nvrhi/src/d3d11/d3d11-compute.cpp",
			"nvrhi/src/d3d11/d3d11-constants.cpp",
			"nvrhi/src/d3d11/d3d11-backend.h",
			"nvrhi/src/d3d11/d3d11-device.cpp",
			"nvrhi/src/d3d11/d3d11-graphics.cpp",
			"nvrhi/src/d3d11/d3d11-queries.cpp",
			"nvrhi/src/d3d11/d3d11-resource-bindings.cpp",
			"nvrhi/src/d3d11/d3d11-shader.cpp",
			"nvrhi/src/d3d11/d3d11-texture.cpp",
		}

		defines {
		}

--		links {
--			"d3d11",
--			"dxguid",
--		}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"

	filter "system:macosx"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "On"
