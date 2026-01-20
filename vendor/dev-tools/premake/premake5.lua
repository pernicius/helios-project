project "Premake"
	kind "Utility"

	dir_project = "%{string.lower(prj.name)}"
	targetdir   (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir      (dir_build .. dir_group .. dir_config .. dir_project)
	
	-- move project in the correct dir
	basedir("../../../")

	files {
		"%{wks.location}/**premake5*.lua"
	}

	postbuildmessage "Regenerating project files with Premake5!"
	filter { "system:windows" }
		postbuildcommands {
			"cd \"%{wks.location}\"",
			"\"vendor/dev-tools/premake/windows/premake5.exe\" %{_ACTION} --file=\"premake5.lua\"",
		}
	filter { "system:linux" }
		postbuildcommands {
			"cd \"%{wks.location}\"",
			"\"vendor/dev-tools/premake/linux/premake5\" %{_ACTION} --file=\"premake5.lua\"",
		}
	filter { "system:macos" }
		postbuildcommands {
			"cd \"%{wks.location}\"",
			"\"vendor/dev-tools/premake/macos/premake5\" %{_ACTION} --file=\"premake5.lua\"",
		}
	filter {}
