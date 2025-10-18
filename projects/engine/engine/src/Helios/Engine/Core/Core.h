#pragma once

// Platform related includes
#include <Platform/PlatformDetection.h>
#if defined TARGET_PLATFORM_WINDOWS
#	include "Platform/System/Windows/WinMain.h"
#	include "Platform/System/Windows/WinUtil.h"
#elif defined TARGET_PLATFORM_LINUX
#	include "Platform/System/Linux/LinuxMain.h"
#	include "Platform/System/Linux/LinuxUtil.h"
#elif defined TARGET_PLATFORM_MACOS
#	include "Platform/System/MacOS/MacOSMain.h"
#	include "Platform/System/MacOS/MacOSUtil.h"
#endif

// Shared includes
#include <Helios/Util/ScopeRef.h>
#include <Helios/Util/Version.h>

// Engine includes
#include "Helios/Engine/Core/Version.h"

// Library includes
#include <GLFW/glfw3.h>

// C/C++ includes

