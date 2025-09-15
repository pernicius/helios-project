#pragma once


// Version
#include "Helios/Engine/Version.h"


// Configuration
#include "Helios/Engine/Config.h"


// Platform
#include "Platform/PlatformDetection.h"
#if defined(TARGET_PLATFORM_WINDOWS)
#	include "Platform/System/Windows/WinMain.h"
#elif defined(TARGET_PLATFORM_LINUX)
#	include "Platform/System/Linux/LinuxMain.h"
#elif defined(TARGET_PLATFORM_MACOS)
#	include "Platform/System/MacOS/MacOSMain.h"
#endif


// Standart C/C++
#include <algorithm>
#include <utility>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>

#include <fstream>
#include <iostream>
#include <sstream>

#include <array>
#include <string>

#include <tuple>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>


// Engine Misc
#include "Helios/Engine/Core/ScopeRef.h"
#include "Helios/Engine/Core/Log.h"
//#include "Helios/Engine/Debug/Instrumentor.h"
#include "Helios/Engine/Core/Util.h"


// Renderer
#ifdef HE_RENDERER_DX11
#	include <DXGI.h>
#	include <d3d11.h>
#endif
#ifdef HE_RENDERER_DX12
#	include <DXGI.h>
#	include <d3d12.h>
#endif
#ifdef HE_RENDERER_VULKAN
#	include <nvrhi/vulkan.h>
#endif
#include <nvrhi/nvrhi.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


#if 0
// Libraries (GLM)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#endif


// Libraries (EnTT)
//#include <entt/entt.hpp>


// Libraries (other)
//#include <Common/Misc.h>
