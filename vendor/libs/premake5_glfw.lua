-- TODO: how to use/import a cmake project?
-- TODO: how to use/import a cmake project?
-- TODO: how to use/import a cmake project?


-------------------------
-- [DEPENDANCY HELPER] --
-------------------------
function Lib_GLFW()
	includedirs "%{wks.location}/vendor/libs/glfw/include/"

	filter "kind:*App"
		links "vendor.glfw"
	filter {}
end


-----------------------
-- [ PROJECT CONFIG] --
-----------------------
project "vendor.glfw"
	kind          "StaticLib"
	architecture  "x86_64"
	language      "C"
	cppdialect    "C++20"
	staticruntime "On"
	targetname    "glfw3"
	
	dir_project = "%{string.lower(prj.name)}"
	targetdir (dir_bin   .. dir_group .. dir_config .. dir_project)
	objdir    (dir_build .. dir_group .. dir_config .. dir_project)

	-- move project in the correct dir
	basedir("glfw")
	
	includedirs {
		"glfw/include",
		"glfw/src"
	}
	
	files {
		"glfw/include/GLFW/glfw3.h",
		"glfw/include/GLFW/glfw3native.h",
		"glfw/src/internal.h",
		"glfw/src/platform.h",
		"glfw/src/mappings.h",
		"glfw/src/context.c",
		"glfw/src/init.c",
		"glfw/src/input.c",
		"glfw/src/monitor.c",
		"glfw/src/platform.c",
		"glfw/src/vulkan.c",
		"glfw/src/window.c",
		"glfw/src/egl_context.c",
		"glfw/src/osmesa_context.c",
		"glfw/src/null_platform.h",
		"glfw/src/null_joystick.h",
		"glfw/src/null_init.c",
		"glfw/src/null_monitor.c",
		"glfw/src/null_window.c",
		"glfw/src/null_joystick.c"
	}

--[[
	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<
	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<
	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"

		files {
			-- !APPLE && !WIN32
			"glfw/src/posix_time.h",
			"glfw/src/posix_thread.h",
			"glfw/src/posix_module.c",
			"glfw/src/posix_time.c",
			"glfw/src/posix_thread.c".
			-- GLFW_BUILD_X11
			"glfw/x11_platform.h",
			"glfw/src/xkb_unicode.h",
			"glfw/src/x11_init.c",
			"glfw/src/x11_monitor.c",
			"glfw/src/x11_window.c",
			"glfw/src/xkb_unicode.c",
			"glfw/src/glx_context.c",
			-- "Linux"
			"glfw/src/linux_joystick.h"
			"glfw/src/linux_joystick.c",
			-- GLFW_BUILD_WAYLAND
			"glfw/src/wl_platform.h"
			"glfw/src/wl_init.c"
			"glfw/src/wl_monitor.c"
			"glfw/src/wl_window.c"
			-- GLFW_BUILD_X11 OR GLFW_BUILD_WAYLAND
			"glfw/src/posix_poll.h",
			"glfw/src/posix_poll.c"
		}

		defines {
			"_GLFW_X11",
			"_DEFAULT_SOURCE"
		}

	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<
	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<
	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<
]]--

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		files {
			-- WIN32
			"glfw/src/win32_time.h",
			"glfw/src/win32_thread.h",
			"glfw/src/win32_module.c",
			"glfw/src/win32_time.c",
			"glfw/src/win32_thread.c",
			-- GLFW_BUILD_WIN32
			"glfw/src/win32_platform.h",
			"glfw/src/win32_joystick.h",
			"glfw/src/win32_init.c",
			"glfw/src/win32_joystick.c",
			"glfw/src/win32_monitor.c",
			"glfw/src/win32_window.c",
			"glfw/src/wgl_context.c"
		}

		defines {
			"_GLFW_WIN32",
			"UNICODE", "_UNICODE",
			"_CRT_SECURE_NO_WARNINGS"
		}

--[[
	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<
	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<
	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<

	filter "system:macosx"

		files {
			-- APPLE
			glfw/src/cocoa_time.h,
			glfw/src/cocoa_time.c,
			glfw/src/posix_thread.h,
			glfw/src/posix_module.c,
			glfw/src/posix_thread.c,
			-- GLFW_BUILD_COCOA
			glfw/src/cocoa_platform.h,
			glfw/src/cocoa_joystick.h,
			glfw/src/cocoa_init.m,
			glfw/src/cocoa_joystick.m,
			glfw/src/cocoa_monitor.m,
			glfw/src/cocoa_window.m,
			glfw/src/nsgl_context.m
		}

		defines {
			_GLFW_COCOA
		}

	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<
	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<
	TODO: >>>>>> I CAN'T TEST THIS PART <<<<<<
]]--

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		
	filter {}
