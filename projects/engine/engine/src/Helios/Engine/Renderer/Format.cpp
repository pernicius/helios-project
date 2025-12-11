#include "pch.h"
#include "Helios/Engine/Renderer/Format.h"

namespace Helios::Engine::Renderer {


	// Helper to combine FormatInfoFlags into the uint8_t storage used by FormatInfo::flags.
	constexpr auto MakeFlags = [](auto... flags) -> uint8_t {
		return (0u | ... | static_cast<uint8_t>(flags));
	};


	// Format mapping table. Rows must be in the exact same order as Format enum members.
	static const FormatInfo c_FormatInfo[] = {
		//  Format                   Name                Bpb  Bs  Kind                      R  G  B  A  D  S   Flags
		{ Format::UNKNOWN,           "UNKNOWN",           0,   0, FormatKind::Integer,      0, 0, 0, 0, 0, 0,  0 },

		{ Format::R8_UINT,           "R8_UINT",           1,   1, FormatKind::Integer,      8, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed) },
		{ Format::R8_SINT,           "R8_SINT",           1,   1, FormatKind::Integer,      8, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::IsSigned) },
		{ Format::R8_UNORM,          "R8_UNORM",          1,   1, FormatKind::Normalized,   8, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed) },
		{ Format::R8_SNORM,          "R8_SNORM",          1,   1, FormatKind::Normalized,   8, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::IsSigned) },

		{ Format::RG8_UINT,          "RG8_UINT",          2,   1, FormatKind::Integer,      8, 8, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen) },
		{ Format::RG8_SINT,          "RG8_SINT",          2,   1, FormatKind::Integer,      8, 8, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::IsSigned) },
		{ Format::RG8_UNORM,         "RG8_UNORM",         2,   1, FormatKind::Normalized,   8, 8, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen) },
		{ Format::RG8_SNORM,         "RG8_SNORM",         2,   1, FormatKind::Normalized,   8, 8, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::IsSigned) },

		{ Format::R16_UINT,          "R16_UINT",          2,   1, FormatKind::Integer,      16,0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed) },
		{ Format::R16_SINT,          "R16_SINT",          2,   1, FormatKind::Integer,      16,0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::IsSigned) },
		{ Format::R16_UNORM,         "R16_UNORM",         2,   1, FormatKind::Normalized,   16,0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed) },
		{ Format::R16_SNORM,         "R16_SNORM",         2,   1, FormatKind::Normalized,   16,0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::IsSigned) },
		{ Format::R16_FLOAT,         "R16_FLOAT",         2,   1, FormatKind::Float,        16,0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::IsSigned) },

		{ Format::BGRA4_UNORM,       "BGRA4_UNORM",       2,   1, FormatKind::Normalized,   4, 4, 4, 4, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::B5G6R5_UNORM,      "B5G6R5_UNORM",      2,   1, FormatKind::Normalized,   5, 6, 5, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue) },
		{ Format::B5G5R5A1_UNORM,    "B5G5R5A1_UNORM",    2,   1, FormatKind::Normalized,   5, 5, 5, 1, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },

		{ Format::RGBA8_UINT,        "RGBA8_UINT",        4,   1, FormatKind::Integer,      8, 8, 8, 8, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::RGBA8_SINT,        "RGBA8_SINT",        4,   1, FormatKind::Integer,      8, 8, 8, 8, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSigned) },
		{ Format::RGBA8_UNORM,       "RGBA8_UNORM",       4,   1, FormatKind::Normalized,   8, 8, 8, 8, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::RGBA8_SNORM,       "RGBA8_SNORM",       4,   1, FormatKind::Normalized,   8, 8, 8, 8, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSigned) },

		{ Format::BGRA8_UNORM,       "BGRA8_UNORM",       4,   1, FormatKind::Normalized,   8, 8, 8, 8, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::SRGBA8_UNORM,      "SRGBA8_UNORM",      4,   1, FormatKind::Normalized,   8, 8, 8, 8, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSRGB) },
		{ Format::SBGRA8_UNORM,      "SBGRA8_UNORM",      4,   1, FormatKind::Normalized,   8, 8, 8, 8, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },

		{ Format::R10G10B10A2_UNORM, "R10G10B10A2_UNORM", 4,   1, FormatKind::Normalized,   10,10,10,2, 0, 0, MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::R11G11B10_FLOAT,   "R11G11B10_FLOAT",   4,   1, FormatKind::Float,        11,11,10,0, 0, 0, MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue) },

		{ Format::RG16_UINT,         "RG16_UINT",         4,   1, FormatKind::Integer,      16,16,0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen) },
		{ Format::RG16_SINT,         "RG16_SINT",         4,   1, FormatKind::Integer,      16,16,0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::IsSigned) },
		{ Format::RG16_UNORM,        "RG16_UNORM",        4,   1, FormatKind::Normalized,   16,16,0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen) },
		{ Format::RG16_SNORM,        "RG16_SNORM",        4,   1, FormatKind::Normalized,   16,16,0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::IsSigned) },
		{ Format::RG16_FLOAT,        "RG16_FLOAT",        4,   1, FormatKind::Float,        16,16,0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::IsSigned) },

		{ Format::R32_UINT,          "R32_UINT",          4,   1, FormatKind::Integer,      32,0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed) },
		{ Format::R32_SINT,          "R32_SINT",          4,   1, FormatKind::Integer,      32,0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::IsSigned) },
		{ Format::R32_FLOAT,         "R32_FLOAT",         4,   1, FormatKind::Float,        32,0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::IsSigned) },

		{ Format::RGBA16_UINT,       "RGBA16_UINT",       8,   1, FormatKind::Integer,      16,16,16,16,0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::RGBA16_SINT,       "RGBA16_SINT",       8,   1, FormatKind::Integer,      16,16,16,16,0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSigned) },
		{ Format::RGBA16_FLOAT,      "RGBA16_FLOAT",      8,   1, FormatKind::Float,        16,16,16,16,0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSigned) },
		{ Format::RGBA16_UNORM,      "RGBA16_UNORM",      8,   1, FormatKind::Normalized,   16,16,16,16,0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::RGBA16_SNORM,      "RGBA16_SNORM",      8,   1, FormatKind::Normalized,   16,16,16,16,0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSigned) },

		{ Format::RG32_UINT,         "RG32_UINT",         8,   1, FormatKind::Integer,      32,32,0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen) },
		{ Format::RG32_SINT,         "RG32_SINT",         8,   1, FormatKind::Integer,      32,32,0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::IsSigned) },
		{ Format::RG32_FLOAT,        "RG32_FLOAT",        8,   1, FormatKind::Float,        32,32,0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::IsSigned) },

		{ Format::RGB32_UINT,        "RGB32_UINT",        12,  1, FormatKind::Integer,      32,32,32,0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue) },
		{ Format::RGB32_SINT,        "RGB32_SINT",        12,  1, FormatKind::Integer,      32,32,32,0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::IsSigned) },
		{ Format::RGB32_FLOAT,       "RGB32_FLOAT",       12,  1, FormatKind::Float,        32,32,32,0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::IsSigned) },

		{ Format::RGBA32_UINT,       "RGBA32_UINT",       16,  1, FormatKind::Integer,      32,32,32,32,0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::RGBA32_SINT,       "RGBA32_SINT",       16,  1, FormatKind::Integer,      32,32,32,32,0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSigned) },
		{ Format::RGBA32_FLOAT,      "RGBA32_FLOAT",      16,  1, FormatKind::Float,        32,32,32,32,0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSigned) },

		{ Format::D16,               "D16",               2,   1, FormatKind::DepthStencil, 0, 0, 0, 0, 16,0,  MakeFlags(FormatInfoFlags::HasDepth) },
		{ Format::D24S8,             "D24S8",             4,   1, FormatKind::DepthStencil, 0, 0, 0, 0, 24,8,  MakeFlags(FormatInfoFlags::HasDepth, FormatInfoFlags::HasStencil) },
		{ Format::X24G8_UINT,        "X24G8_UINT",        4,   1, FormatKind::Integer,      0, 0, 0, 0, 0, 8,  MakeFlags(FormatInfoFlags::HasStencil) },
		{ Format::D32,               "D32",               4,   1, FormatKind::DepthStencil, 0, 0, 0, 0, 32,0,  MakeFlags(FormatInfoFlags::HasDepth) },
		{ Format::D32S8,             "D32S8",             8,   1, FormatKind::DepthStencil, 0, 0, 0, 0, 32,8,  MakeFlags(FormatInfoFlags::HasDepth, FormatInfoFlags::HasStencil) },
		{ Format::X32G8_UINT,        "X32G8_UINT",        8,   1, FormatKind::Integer,      0, 0, 0, 0, 0, 8,  MakeFlags(FormatInfoFlags::HasStencil) },

		// Block-compressed formats: per-pixel bit counts are not directly applicable -> set 0
		{ Format::BC1_UNORM,         "BC1_UNORM",         8,   4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::BC1_UNORM_SRGB,    "BC1_UNORM_SRGB",    8,   4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSRGB) },
		{ Format::BC2_UNORM,         "BC2_UNORM",         16,  4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::BC2_UNORM_SRGB,    "BC2_UNORM_SRGB",    16,  4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSRGB) },
		{ Format::BC3_UNORM,         "BC3_UNORM",         16,  4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::BC3_UNORM_SRGB,    "BC3_UNORM_SRGB",    16,  4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSRGB) },

		{ Format::BC4_UNORM,         "BC4_UNORM",         8,   4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed) },
		{ Format::BC4_SNORM,         "BC4_SNORM",         8,   4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::IsSigned) },
		{ Format::BC5_UNORM,         "BC5_UNORM",         16,  4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen) },
		{ Format::BC5_SNORM,         "BC5_SNORM",         16,  4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::IsSigned) },

		{ Format::BC6H_UFLOAT,       "BC6H_UFLOAT",       16,  4, FormatKind::Float,        0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue) },
		{ Format::BC6H_SFLOAT,       "BC6H_SFLOAT",       16,  4, FormatKind::Float,        0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::IsSigned) },

		{ Format::BC7_UNORM,         "BC7_UNORM",         16,  4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha) },
		{ Format::BC7_UNORM_SRGB,    "BC7_UNORM_SRGB",    16,  4, FormatKind::Normalized,   0, 0, 0, 0, 0, 0,  MakeFlags(FormatInfoFlags::HasRed, FormatInfoFlags::HasGreen, FormatInfoFlags::HasBlue, FormatInfoFlags::HasAlpha, FormatInfoFlags::IsSRGB) },
	};


	const FormatInfo& GetFormatInfo(Format format)
	{
		static_assert(sizeof(c_FormatInfo) / sizeof(FormatInfo) == size_t(Format::COUNT),
			"The format info table doesn't have the right number of elements");

		if (uint32_t(format) >= uint32_t(Format::COUNT))
			return c_FormatInfo[0]; // UNKNOWN

		const FormatInfo& info = c_FormatInfo[uint32_t(format)];
		assert(info.format == format);
		return info;
	}


} // namespace Helios::Engine::Renderer
