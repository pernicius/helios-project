-------------------------
-- [DEPENDANCY HELPER] --
-------------------------
function Lib_glad()
	includedirs "%{wks.location}/vendor/libs/glad/include/"

	filter "kind:*App"
		links "vendor.glad"
	filter {}
end


-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "vendor.glad"
	kind          "StaticLib"
	architecture  "x86_64"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	targetname    "glad"
	
	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	-- move project in the correct dir
	basedir("glad")
	
	includedirs {
		"glad/include",
		"glad/src"
	}
	
	files {
		"premake5_glad.lua",

		"glad/include/**.h",
		"glad/src/**.c",
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		
	filter {}
