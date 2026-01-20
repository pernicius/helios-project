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
	
	-- move project in the correct dir
	basedir("spdlog")
	
	files {
		"premake5_spdlog.lua",
		"spdlog/include/**.h",
	}
