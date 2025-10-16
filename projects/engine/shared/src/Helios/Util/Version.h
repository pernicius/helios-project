#pragma once

#include <cstdint>


namespace Helios::Version {
	enum Type
	{
		Unknown = 0,
		Debug,
		//		Alpha,
		//		Beta,
		//		ReleaseCandidate,
		Release
	};
	namespace Max {
		constexpr uint32_t Type  = 7;    // 3 bits
		constexpr uint32_t Major = 127;  // 7 bits
		constexpr uint32_t Minor = 1023; // 10 bits
		constexpr uint32_t Patch = 4095; // 12 bits
	};
}


// This will create an uint32_t with the spezified version
//  -  3 bits: Helios::Util::VersionType
//  -  7 bits: major version
//  - 10 bits: minor version
//  - 12 bits: patch number
#ifdef BUILD_DEBUG
#	define HE_MAKE_VERSION(major, minor, patch) ((((uint32_t)(Helios::Version::Type::Debug)) << 29U) |\
		(((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))
#else
#	define HE_MAKE_VERSION(major, minor, patch) ((((uint32_t)(Helios::Version::Type::Release)) << 29U) |\
		(((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))
#endif


#define HE_VERSION_TYPE(version)  ((uint32_t)(version) >> 29U)
#define HE_VERSION_MAJOR(version) (((uint32_t)(version) >> 22U) & 0x7FU)
#define HE_VERSION_MINOR(version) (((uint32_t)(version) >> 12U) & 0x3FFU)
#define HE_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFFU)


#define HE_VERSION_TYPE_STRING(version) (\
	(HE_VERSION_TYPE(version) == Helios::Version::Type::Debug)   ? "Debug" : \
	(HE_VERSION_TYPE(version) == Helios::Version::Type::Release) ? "Release" : \
	"Unknown")
