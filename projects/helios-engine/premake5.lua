-------------------------
-- [DEPENDANCY HELPER] --
-------------------------
include "premake5_renderer.lua"

function Lib_HeliosEngine()
	includedirs "%{wks.location}/projects/helios-engine/src/"

	filter "kind:*App"
		links "Helios-Engine"
	filter {}

	-- Renderer
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
	language              "C++"
	cppdialect            "C++20"
	staticruntime         "On"
	kind                  "StaticLib"

	dir_project = ("%{string.lower(prj.name)}" .. "/")
	targetdir   (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir      (dir_build .. dir_group .. dir_config .. dir_project)

	pchheader "pch.h"
	pchsource "pch.cpp"

	includedirs {
		"%{prj.location}",
		"src/",
	}

	-- Files used on all platforms
	files {
		-- premake files
		"premake5*.lua",
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
	-- Remove all platform files, platform specific ones will be added later
	removefiles {
		"src/Helios/Platform/Renderer/**.h",
		"src/Helios/Platform/Renderer/**.cpp",
		"src/Helios/Platform/System/**.h",
		"src/Helios/Platform/System/**.cpp",
	}

	-- Platform specific (Windows)
	filter "platforms:Windows"
		files {
			"src/Helios/Platform/System/Windows/**.h",
			"src/Helios/Platform/System/Windows/**.cpp",
		}
	-- Platform -pecific (Linux)
	filter "platforms:Linux"
		files {
			"src/Helios/Platform/System/Linux/**.h",
			"src/Helios/Platform/System/Linux/**.cpp",
		}
	-- Platform specific (MacOS)
	filter "platforms:MacOS"
		files {
			"src/Helios/Platform/System/MacOS/**.h",
			"src/Helios/Platform/System/MacOS/**.cpp",
		}
	filter {}

	-- Renderer specific
	Src_RendererVulkan{}
	Src_RendererDirectX12{}
	Src_RendererMetal{}

	-- Dependencies
	Lib_HeliosEngine{}
	
	-- Debug/Release Configuration
	filter "configurations:Debug"
		defines {
		}
	filter "configurations:Release"
		defines {
		}
	filter {}

	-- Shader compilation (GLSL -> SPIR-V (glslangValidator) -> HLSL (spirv-cross) -> DXIL/CSO (dxc))
	filter { "system:windows", "files:**.glsl" }
		buildmessage "Compiling %{file.name} -> %{file.basename}.spv"
		buildcommands {
			"\"%{(os.getenv('VULKAN_SDK') .. '/Bin/glslangValidator.exe')}\" -V \"%{file.relpath}\" -o \"%{(cfg.buildtarget.directory .. '/assets/shaders/vulkan/' .. file.basename)}.spv\"",
			-- glslangValidator.exe leaves *.spv files in the working directory
			"del *.spv",
		}
		buildoutputs {
			"%{(cfg.buildtarget.directory .. '/assets/shaders/vulkan/' .. file.basename)}.spv",
		}
--	filter { "system:linux or macos", "files:**.glsl" }
--		buildmessage "Compiling %{file.name} -> %{file.basename}.spv"
--		buildcommands {
--			"\"%{os.getenv('VULKAN_SDK')}/bin/glslangValidator\" -V \"%{file.relpath}\" -o \"%{cfg.buildtarget.directory}assets/shader/vulkan/%{file.basename}.spv\"",
--			-- TODO: check if glslangValidator leaves .spv files in the working directory on Linux/MacOS like it does on Windows
--			"rm *.spv"
--		}
--		buildoutputs {
--			"%{cfg.buildtarget.directory}assets/shader/vulkan/%{file.basename}.spv"
--		}
	filter {}
