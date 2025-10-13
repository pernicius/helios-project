#pragma once
// ============================================================================
// The following code will define some symbols based on predefined macros.
//
// Currently suported:
//   TARGET_PLATFORM_WINDOWS
//
// Probably supported in the feature:
//   TARGET_PLATFORM_LINUX
//   TARGET_PLATFORM_MACOS
// 
// Not supported but detected:
//   TARGET_PLATFORM_IOS_SIMULATOR
//   TARGET_PLATFORM_IOS
//   TARGET_PLATFORM_ANDROID
// ============================================================================


#ifdef _WIN32
#	ifdef _WIN64
		// Windows x64
#		define TARGET_PLATFORM_WINDOWS
#	else
		// Windows x86
#		error "x86 Builds (32bits) are not supported!"
#	endif

// ----------------------------------------------------------------------------

#elif defined(__APPLE__) || defined(__MACH__)
#	include <TargetConditionals.h>
	// TARGET_OS_MAC exists on all the platforms
	// so we must check all of them (in this order)
	// to ensure that we're running on MAC
	// and not some other Apple platform
#	if TARGET_IPHONE_SIMULATOR == 1
		// Apple IOS Simulator
#		define TARGET_PLATFORM_IOS_SIMULATOR
#		define TARGET_PLATFORM_IOS
#		error "IOS simulator is not supported!"
#	elif TARGET_OS_IPHONE == 1
		// Apple IPhone
#		define TARGET_PLATFORM_IOS
#		error "IOS is not supported!"
#	elif TARGET_OS_MAC == 1
		// Apple MacOS
#		define TARGET_PLATFORM_MACOS
#	else
#		error "Unknown Apple platform!"
#	endif

// ----------------------------------------------------------------------------

// We also have to check __ANDROID__ before __linux__
// since android is based on the linux kernel
// it has __linux__ defined
#elif defined(__ANDROID__)
	// Android
#	define TARGET_PLATFORM_ANDROID
#	error "Android is not supported!"

// ----------------------------------------------------------------------------

#elif defined(__linux__)
	// Linux
#	define TARGET_PLATFORM_LINUX

// ----------------------------------------------------------------------------

#else
	// Unknown compiler/platform
#	error "Unknown platform!"
#endif
