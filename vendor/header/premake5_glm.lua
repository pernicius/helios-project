-------------------------
-- [DEPENDANCY HELPER] --
-------------------------


function Lib_GLM()
	includedirs "%{wks.location}/vendor/header/glm/"
end


-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "vendor.glm"
	kind "None"
	
	-- move project in the correct dir
	basedir("glm")
	
	files {
		"premake5_glm.lua",
		"glm/glm/**.*",
	}
