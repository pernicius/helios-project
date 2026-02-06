-------------------------
-- [DEPENDANCY HELPER] --
-------------------------


function Lib_GLM()
	includedirs "%{wks.location}/vendor/header/glm/"
	defines {
		"GLM_FORCE_RADIANS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
	}
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
