-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "nvrhi.common"
	kind          "StaticLib"
	architecture  "x86_64"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	targetname    "nvrhi"
	
	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	-- move project in the correct dir
	basedir("nvrhi")
	
	includedirs {
		"nvrhi/include",
		"nvrhi/src",
	}
	
	files {
		-- include
		"nvrhi/include/nvrhi/nvrhi.h",
		"nvrhi/include/nvrhi/nvrhiHLSL.h",
		"nvrhi/include/nvrhi/utils.h",
		"nvrhi/include/nvrhi/common/containers.h",
		"nvrhi/include/nvrhi/common/misc.h",
		"nvrhi/include/nvrhi/common/resource.h",
		"nvrhi/include/nvrhi/common/aftermath.h",
		-- src
		"nvrhi/src/common/format-info.cpp",
		"nvrhi/src/common/misc.cpp",
		"nvrhi/src/common/state-tracking.cpp",
		"nvrhi/src/common/state-tracking.h",
		"nvrhi/src/common/utils.cpp",
		"nvrhi/src/common/aftermath.cpp",
	}

	defines {
		"NVRHI_WITH_AFTERMATH=0",
		"NVRHI_D3D11_WITH_NVAPI=0",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		files {
		}

		defines {
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
