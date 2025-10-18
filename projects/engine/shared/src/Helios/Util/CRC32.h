#pragma once

#include <cstdint>
#include <array>
#include <span>

namespace Helios::Util {


	class CRC32
	{
	public:
		static constexpr uint32_t DefaultPolynomial = 0xEDB88320u;
		static constexpr uint32_t DefaultSeed = 0xFFFFFFFFu;

		CRC32() = default;

		static uint32_t Compute(std::span<const uint8_t> data, uint32_t seed = DefaultSeed)
		{
			static const auto table = GenerateTable(DefaultPolynomial);
			uint32_t crc = seed;
			for (uint8_t byte : data)
			{
				crc = table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
			}
			return crc ^ 0xFFFFFFFFu;
		}

	private:
		static constexpr std::array<uint32_t, 256> GenerateTable(uint32_t polynomial)
		{
			std::array<uint32_t, 256> table{};
			for (uint32_t i = 0; i < 256; ++i)
			{
				uint32_t c = i;
				for (size_t j = 0; j < 8; ++j)
				{
					if (c & 1)
						c = polynomial ^ (c >> 1);
					else
						c >>= 1;
				}
				table[i] = c;
			}
			return table;
		}
	};


} // namespace Helios::Util
