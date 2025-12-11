-------------------------
-- [DEPENDANCY HELPER] --
-------------------------
include "premake5_renderer"

function Lib_HeliosEngine()
	includedirs "%{wks.location}/projects/engine/engine/src/"

	filter "kind:*App"
		links "Helios-Engine"
	filter {}

	Lib_HeliosShared{}

	-- Renderer
--	Lib_RendererOpenGL{}
	Lib_RendererVulkan{}

	-- vendor/header
	Lib_Spdlog{}

	-- vendor/lib
	Lib_GLFW{}
end


----------------------
-- [PROJECT CONFIG] --
----------------------
project "Helios-Engine"
	language      "C++"
	cppdialect    "C++20"
	staticruntime "On"
	kind          "StaticLib"

	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	pchheader "pch.h"
	pchsource "pch.cpp"

	includedirs {
		"%{prj.location}",
		"src/",
	}

	files {
		-- premake files
		"premake5.lua",
		"premake5_renderer.lua",
		-- c/c++ files
		"pch.*",
		"src/Helios/**.h",
		"src/Helios/**.c",
		"src/Helios/**.hpp",
		"src/Helios/**.cpp",
		-- shaders
		"src/Helios/**.vert.glsl",
		"src/Helios/**.frag.glsl",
	}
--	Src_RendererOpenGL{}
	Src_RendererVulkan{}

	filter "platforms:Windows"

		files {
			"src/Platform/System/Windows/**.h",
			"src/Platform/System/Windows/**.cpp",
		}

	filter "platforms:Linux"

		files {
			"src/Platform/System/Linux/**.h",
			"src/Platform/System/Linux/**.cpp",
		}

	filter "platforms:MacOS"

		files {
			"src/Platform/System/MacOS/**.h",
			"src/Platform/System/MacOS/**.cpp",
		}

	filter {}

	-- Dependencies
	Lib_HeliosEngine{}
	
	filter "configurations:Debug"

		defines {
		}

	filter "configurations:Release"

		defines {
		}

	filter {}

	-- Shader Compilation (GLSL -> SPIR-V (glslangValidator) -> HLSL (spirv-cross) -> DXIL/CSO (dxc))
	-- TODO: check the DXIL/CSO part
	filter { "system:windows", "files:**.glsl" }
		buildmessage "Compiling %{file.name} -> %{file.basename}.spv"
		buildcommands {
			"\"%{os.getenv('VULKAN_SDK')}\\Bin\\glslangValidator.exe\" -V \"%{file.relpath}\" -o \"%{cfg.buildtarget.directory}assets\\shader\\vulkan\\%{file.basename}.spv\"",
			"del *.spv",
			"\"%{os.getenv('VULKAN_SDK')}\\Bin\\spirv-cross.exe\" \"%{cfg.buildtarget.directory}assets\\shader\\vulkan\\%{file.basename}.spv\" --hlsl --shader-model 60 --output \"%{cfg.objdir}assets\\shader\\%{file.basename}.hlsl\"",
--			"\"dxc.exe\" -T vs_6_0 -E main -Fo \"%{cfg.buildtarget.directory}assets\\shader\\dx12\\%{file.basename}.cso\" \"%{cfg.objdir}assets\\shader\\%{file.basename}.hlsl\""
		}
		buildoutputs {
			"%{cfg.buildtarget.directory}\\assets\\shader\\vulkan\\%{file.basename}.spv",
			"%{cfg.objdir}\\assets\\shader\\%{file.basename}.hlsl",
--			"%{cfg.buildtarget.directory}\\assets\\shader\\dx12\\%{file.basename}.cso"
		}
	
	filter { "system:linux or macos", "files:**.glsl" }
		buildmessage "Compiling %{file.name} -> %{file.basename}.spv"
		buildcommands {
			"\"%{os.getenv('VULKAN_SDK')}/bin/glslangValidator\" -V \"%{file.relpath}\" -o \"%{cfg.buildtarget.directory}assets/shader/vulkan/%{file.basename}.spv\"",
			-- TODO: check if glslangValidator leaves .spv files in the working directory on Linux/MacOS like it does on Windows
			"rm *.spv"
		}
		buildoutputs { "%{cfg.buildtarget.directory}assets/shader/vulkan/%{file.basename}.spv" }

	filter {}
