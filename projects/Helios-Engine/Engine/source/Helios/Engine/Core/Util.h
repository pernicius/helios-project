#pragma once


#if defined TARGET_PLATFORM_WINDOWS
#	include "Platform/System/Windows/WinUtil.h"
#elif defined TARGET_PLATFORM_LINUX
#	include "Platform/System/Linux/LinuxUtil.h"
#elif defined TARGET_PLATFORM_MACOS
#	include "Platform/System/MacOS/MacOSUtil.h"
#endif


namespace Helios::Util {


	std::string GetExecutablePath();


} // namespace Helios::Util
