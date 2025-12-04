#pragma once

#include <cstdint>

namespace Helios::Engine::Renderer {


	enum class Format : uint8_t
	{
		UNKNOWN,

		R8_UINT,
		R8_SINT,
		R8_UNORM,
		R8_SNORM,
		RG8_UINT,
		RG8_SINT,
		RG8_UNORM,
		RG8_SNORM,
		R16_UINT,
		R16_SINT,
		R16_UNORM,
		R16_SNORM,
		R16_FLOAT,
		BGRA4_UNORM,
		B5G6R5_UNORM,
		B5G5R5A1_UNORM,
		RGBA8_UINT,
		RGBA8_SINT,
		RGBA8_UNORM,
		RGBA8_SNORM,
		BGRA8_UNORM,
		SRGBA8_UNORM,
		SBGRA8_UNORM,
		R10G10B10A2_UNORM,
		R11G11B10_FLOAT,
		RG16_UINT,
		RG16_SINT,
		RG16_UNORM,
		RG16_SNORM,
		RG16_FLOAT,
		R32_UINT,
		R32_SINT,
		R32_FLOAT,
		RGBA16_UINT,
		RGBA16_SINT,
		RGBA16_FLOAT,
		RGBA16_UNORM,
		RGBA16_SNORM,
		RG32_UINT,
		RG32_SINT,
		RG32_FLOAT,
		RGB32_UINT,
		RGB32_SINT,
		RGB32_FLOAT,
		RGBA32_UINT,
		RGBA32_SINT,
		RGBA32_FLOAT,

		D16,
		D24S8,
		X24G8_UINT,
		D32,
		D32S8,
		X32G8_UINT,

		BC1_UNORM,
		BC1_UNORM_SRGB,
		BC2_UNORM,
		BC2_UNORM_SRGB,
		BC3_UNORM,
		BC3_UNORM_SRGB,
		BC4_UNORM,
		BC4_SNORM,
		BC5_UNORM,
		BC5_SNORM,
		BC6H_UFLOAT,
		BC6H_SFLOAT,
		BC7_UNORM,
		BC7_UNORM_SRGB,

		COUNT,
	};


	enum class FormatKind : uint8_t
	{
		Integer,
		Normalized,
		Float,
		DepthStencil
	};


	enum class FormatInfoFlags : uint8_t
	{
		None = 0,

		HasRed     = 1 << 0,
		HasGreen   = 1 << 1,
		HasBlue    = 1 << 2,
		HasAlpha   = 1 << 3,
		HasDepth   = 1 << 4,
		HasStencil = 1 << 5,

		IsSigned   = 1 << 6,
		IsSRGB     = 1 << 7,
	};


	struct FormatInfo
	{
		Format format;
		const char* name;

		uint8_t bytesPerBlock;
		uint8_t blockSize;
		FormatKind kind;

		// Per-component bit counts (0 if not applicable or compressed)
		uint8_t redBits;
		uint8_t greenBits;
		uint8_t blueBits;
		uint8_t alphaBits;
		uint8_t depthBits;
		uint8_t stencilBits;

		uint8_t flags;
	};


	// Small helpers for checking FormatInfo::flags.
	// - HasFlag: checks a single flag.
	// - HasAllFlags / HasAnyFlag: variadic helpers for convenience.
	inline constexpr bool HasFlag(const FormatInfo& info, FormatInfoFlags flag) noexcept
	{
		return (info.flags & static_cast<uint8_t>(flag)) != 0;
	}

	template<typename... Flags>
	inline constexpr bool HasAllFlags(const FormatInfo& info, Flags... flags) noexcept
	{
		const uint8_t mask = (0u | ... | static_cast<uint8_t>(flags));
		return (info.flags & mask) == mask;
	}

	template<typename... Flags>
	inline constexpr bool HasAnyFlag(const FormatInfo& info, Flags... flags) noexcept
	{
		const uint8_t mask = (0u | ... | static_cast<uint8_t>(flags));
		return (info.flags & mask) != 0;
	}


	const FormatInfo& GetFormatInfo(Format format);


	// Inline helper accessors (definitions call GetFormatInfo overloads when given Format).
	// Per-component bit getters
	inline constexpr uint8_t GetRedBits(const FormatInfo& info) noexcept { return info.redBits; }
	inline constexpr uint8_t GetGreenBits(const FormatInfo& info) noexcept { return info.greenBits; }
	inline constexpr uint8_t GetBlueBits(const FormatInfo& info) noexcept { return info.blueBits; }
	inline constexpr uint8_t GetAlphaBits(const FormatInfo& info) noexcept { return info.alphaBits; }
	inline constexpr uint8_t GetDepthBits(const FormatInfo& info) noexcept { return info.depthBits; }
	inline constexpr uint8_t GetStencilBits(const FormatInfo& info) noexcept { return info.stencilBits; }

	inline constexpr uint8_t GetRedBits(Format fmt) noexcept { return GetRedBits(GetFormatInfo(fmt)); }
	inline constexpr uint8_t GetGreenBits(Format fmt) noexcept { return GetGreenBits(GetFormatInfo(fmt)); }
	inline constexpr uint8_t GetBlueBits(Format fmt) noexcept { return GetBlueBits(GetFormatInfo(fmt)); }
	inline constexpr uint8_t GetAlphaBits(Format fmt) noexcept { return GetAlphaBits(GetFormatInfo(fmt)); }
	inline constexpr uint8_t GetDepthBits(Format fmt) noexcept { return GetDepthBits(GetFormatInfo(fmt)); }
	inline constexpr uint8_t GetStencilBits(Format fmt) noexcept { return GetStencilBits(GetFormatInfo(fmt)); }


	// Bits-per-texel computed from bytesPerBlock/blockSize (safe when blockSize != 0).
	inline constexpr uint32_t BitsPerTexel(const FormatInfo& info) noexcept
	{
		return (info.blockSize == 0) ? 0u : (uint32_t(info.bytesPerBlock) * 8u) / uint32_t(info.blockSize);
	}
	inline constexpr uint32_t BitsPerTexel(Format fmt) noexcept { return BitsPerTexel(GetFormatInfo(fmt)); }


	// Component count (counts red/green/blue/alpha present flags)
	inline constexpr uint8_t ComponentCount(const FormatInfo& info) noexcept
	{
		uint8_t c = 0;
		if (HasFlag(info, FormatInfoFlags::HasRed)) ++c;
		if (HasFlag(info, FormatInfoFlags::HasGreen)) ++c;
		if (HasFlag(info, FormatInfoFlags::HasBlue)) ++c;
		if (HasFlag(info, FormatInfoFlags::HasAlpha)) ++c;
		return c;
	}
	inline constexpr uint8_t ComponentCount(Format fmt) noexcept { return ComponentCount(GetFormatInfo(fmt)); }


} // namespace Helios::Engine::Renderer
