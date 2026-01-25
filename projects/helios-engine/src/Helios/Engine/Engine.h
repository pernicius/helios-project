//==============================================================================
// Engine Master Include Header
//
// Provides a single-include entry point for the Helios Engine. Aggregates all
// essential headers: standard library, third-party dependencies (Vulkan, GLFW),
// platform-specific utilities, core engine systems (Application, Logging,
// Events, Layers), and utility classes (smart pointers, version encoding).
// Include this header in application code to access the full engine API.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
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

