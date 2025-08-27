-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "nvrhi.validation"
	kind          "StaticLib"
	architecture  "x86_64"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	targetname    "nvrhi_validation"
	
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
		"nvrhi/include/nvrhi/validation.h",
		-- src
		"nvrhi/src/validation/validation-commandlist.cpp",
		"nvrhi/src/validation/validation-device.cpp",
		"nvrhi/src/validation/validation-backend.h",
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
