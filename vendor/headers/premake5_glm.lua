-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "vendor.glm"
	kind "Utility"
	
	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)
	
	-- move project in the correct dir
	basedir("glm")

	files {
		"glm/glm/**.*"
	}

	prebuildcommands {
		"{COPYDIR} glm/*.* include/glm"
	}
