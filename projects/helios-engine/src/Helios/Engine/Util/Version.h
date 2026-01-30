//==============================================================================
// Version Encoding Utilities
//
// Provides a compact version encoding scheme that packs version information
// into a single uint32_t: 3 bits for type (Debug/Release), 7 bits for major,
// 10 bits for minor, 12 bits for patch. Includes macros for encoding
// (HE_MAKE_VERSION) and decoding (HE_VERSION_MAJOR, etc.) version numbers.
// Automatically sets version type based on BUILD_DEBUG flag.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Changelog:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include <cstdint>

namespace Helios {


	enum VersionType
	{
		Unknown = 0,
		Debug,
//		Alpha,
//		Beta,
//		ReleaseCandidate,
		Release
	};
	namespace VersionMax {
		constexpr uint32_t Type  = 7;    // 3 bits
		constexpr uint32_t Major = 127;  // 7 bits
		constexpr uint32_t Minor = 1023; // 10 bits
		constexpr uint32_t Patch = 4095; // 12 bits
	};


}


// This will create an uint32_t with the spezified version
//  -  3 bits: Helios::Version::Type
//  -  7 bits: major version
//  - 10 bits: minor version
//  - 12 bits: patch number
#ifdef BUILD_DEBUG
#	define HE_MAKE_VERSION(major, minor, patch) ((((uint32_t)(Helios::VersionType::Debug)) << 29U) |\
		(((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))
#else
#	define HE_MAKE_VERSION(major, minor, patch) ((((uint32_t)(Helios::VersionType::Release)) << 29U) |\
		(((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))
#endif


#define HE_VERSION_TYPE(version)  ((uint32_t)(version) >> 29U)
#define HE_VERSION_MAJOR(version) (((uint32_t)(version) >> 22U) & 0x7FU)
#define HE_VERSION_MINOR(version) (((uint32_t)(version) >> 12U) & 0x3FFU)
#define HE_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFFU)


#define HE_VERSION_TYPE_STRING(version) (\
	(HE_VERSION_TYPE(version) == Helios::VersionType::Debug)   ? "Debug" : \
	(HE_VERSION_TYPE(version) == Helios::VersionType::Release) ? "Release" : \
	"Unknown")
