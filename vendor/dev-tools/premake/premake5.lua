project "Premake"
	kind "Utility"

	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	files {
		"%{wks.location}/**premake5.lua"
	}

	postbuildmessage "Regenerating project files with Premake5!"
	filter { "system:windows" }
		postbuildcommands "\"%{wks.location}vendor/dev-tools/premake/windows/premake5.exe\" %{_ACTION} --file=\"%{wks.location}premake5.lua\""
	filter {}
