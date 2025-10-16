#pragma once
// ============================================================================
// Platform Detection Header
// Defines platform-specific macros for conditional compilation.
// Supported platforms:
//   - Windows (x64)
//   - Linux
//   - MacOS
// Detected but not supported (will trigger compile error):
//   - iOS, iOS Simulator, Android
// ============================================================================

// Undefine all platform macros to avoid accidental redefinition
#undef TARGET_PLATFORM_WINDOWS
#undef TARGET_PLATFORM_LINUX
#undef TARGET_PLATFORM_MACOS
#undef TARGET_PLATFORM_IOS
#undef TARGET_PLATFORM_IOS_SIMULATOR
#undef TARGET_PLATFORM_ANDROID
#undef TARGET_PLATFORM_NAME

// Windows (x64 only)
#if defined(_WIN32)
#   if defined(_WIN64)
#       define TARGET_PLATFORM_WINDOWS 1
#       define TARGET_PLATFORM_NAME "Windows"
#   else
#       error "x86 (32-bit) Windows builds are not supported! Please use a 64-bit toolchain."
#   endif

// Apple platforms
#elif defined(__APPLE__) || defined(__MACH__)
#   include <TargetConditionals.h>
	// TARGET_OS_MAC exists on all the platforms
	// so we must check all of them (in this order)
	// to ensure that we're running on MAC
	// and not some other Apple platform
#   if TARGET_IPHONE_SIMULATOR == 1
#       define TARGET_PLATFORM_IOS_SIMULATOR 1
#       define TARGET_PLATFORM_IOS 1
#       error "iOS Simulator is not supported!"
#   elif TARGET_OS_IPHONE == 1
#       define TARGET_PLATFORM_IOS 1
#       error "iOS is not supported!"
#   elif TARGET_OS_MAC == 1
#       define TARGET_PLATFORM_MACOS 1
#       define TARGET_PLATFORM_NAME "MacOS"
#   else
#       error "Unknown Apple platform! Please update PlatformDetection.h"
#   endif

// Android (must be checked before Linux)
#elif defined(__ANDROID__)
#   define TARGET_PLATFORM_ANDROID 1
#   error "Android is not supported!"

// Linux
#elif defined(__linux__)
#   define TARGET_PLATFORM_LINUX 1
#   define TARGET_PLATFORM_NAME "Linux"

// Unknown platform
#else
#   error "Unknown platform! Please update PlatformDetection.h"
#endif
