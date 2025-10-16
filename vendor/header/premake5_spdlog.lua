-------------------------
-- [DEPENDANCY HELPER] --
-------------------------


function Lib_Spdlog()
	includedirs "%{wks.location}/vendor/header/spdlog/include/"
end


-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "vendor.spdlog"
	kind "None"
	
--	dir_project = "%{string.lower(prj.name)}"
--	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
--	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	-- move project in the correct dir
	basedir("spdlog")
	
--	includedirs {
--		"glfw/include",
--		"glfw/src"
--	}
	
	files {
		"premake5_spdlog.lua",

		"spdlog/include/**.h",
--		"spdlog/src/**.cpp",
	}
