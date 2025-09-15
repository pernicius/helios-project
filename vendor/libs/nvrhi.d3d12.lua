-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "nvrhi.d3d12"
	kind          "StaticLib"
	architecture  "x86_64"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	targetname    "nvrhi_d3d12"
	
	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	-- move project in the correct dir
	basedir("nvrhi")
	
	links { "d3d12", "dxgi", "dxguid" }

	includedirs {
		"nvrhi/include",
		"nvrhi/src",
		-- DirectX-Headers
		"nvrhi/thirdparty/DirectX-Headers/include",
		"nvrhi/thirdparty/DirectX-Headers/include/directx",
	}
	
	files {
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		files {
			-- include
			"nvrhi/include/nvrhi/d3d12.h",
			-- src
			"nvrhi/src/common/dxgi-format.h",
			"nvrhi/src/common/dxgi-format.cpp",
			"nvrhi/src/common/versioning.h",
			"nvrhi/src/d3d12/d3d12-buffer.cpp",
			"nvrhi/src/d3d12/d3d12-commandlist.cpp",
			"nvrhi/src/d3d12/d3d12-compute.cpp",
			"nvrhi/src/d3d12/d3d12-constants.cpp",
			"nvrhi/src/d3d12/d3d12-backend.h",
			"nvrhi/src/d3d12/d3d12-descriptor-heap.cpp",
			"nvrhi/src/d3d12/d3d12-device.cpp",
			"nvrhi/src/d3d12/d3d12-graphics.cpp",
			"nvrhi/src/d3d12/d3d12-meshlets.cpp",
			"nvrhi/src/d3d12/d3d12-queries.cpp",
			"nvrhi/src/d3d12/d3d12-raytracing.cpp",
			"nvrhi/src/d3d12/d3d12-resource-bindings.cpp",
			"nvrhi/src/d3d12/d3d12-shader.cpp",
			"nvrhi/src/d3d12/d3d12-state-tracking.cpp",
			"nvrhi/src/d3d12/d3d12-texture.cpp",
			"nvrhi/src/d3d12/d3d12-upload.cpp",
			-- DirectX-Headers
			"nvrhi/thirdparty/DirectX-Headers/src/d3dx12_property_format_table.cpp",
			"nvrhi/thirdparty/DirectX-Headers/src/dxguids.cpp",
		}

		defines {
		}

--		links {
--			"d3d12",
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
