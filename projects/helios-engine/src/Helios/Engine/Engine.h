#pragma once

// Library includes
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#undef APIENTRY // undefine GLFW's APIENTRY to avoid conflicts

// C includes
#include <cctype>
#include <cstdint>
#include <cstdlib>

// C++ includes
#include <array>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <mutex>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

// Platform related includes
#include <Helios/Platform/PlatformDetection.h>
#if defined TARGET_PLATFORM_WINDOWS
#	include "Helios/Platform/System/Windows/WinMain.h"
#	include "Helios/Platform/System/Windows/WinUtil.h"
#elif defined TARGET_PLATFORM_LINUX
#	include "Helios/Platform/System/Linux/LinuxMain.h"
#	include "Helios/Platform/System/Linux/LinuxUtil.h"
#elif defined TARGET_PLATFORM_MACOS
#	include "Helios/Platform/System/MacOS/MacOSMain.h"
#	include "Helios/Platform/System/MacOS/MacOSUtil.h"
#endif

// Shared includes
#include <Helios/Engine/Util/ScopeRef.h>
#include <Helios/Engine/Util/Version.h>

// Engine includes
#include "Helios/Engine/Core/Log.h"
#include "Helios/Engine/Core/Application.h"
#include "Helios/Engine/Core/Version.h"

