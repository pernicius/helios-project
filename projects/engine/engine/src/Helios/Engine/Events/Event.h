// Original code by Denys Kryvytskyi
// https://github.com/denyskryvytskyi/ElvenEngine/tree/master
#pragma once

#include <Helios/Util/CRC32.h>

// TODO: replace with <fmt/...> when available in the engine
#include <spdlog/fmt/fmt.h>

#include <sstream>
#include <string>

namespace Helios::Engine {


	class Event {
	public:
		virtual ~Event() = default;
		virtual std::uint32_t GetEventType() const = 0;
		virtual std::string ToString() const { return std::to_string(GetEventType()); };

		bool isHandled{ false };
	};

#	define EVENT_TYPE(event_type)                         \
		static std::uint32_t GetStaticEventType()         \
		    { return Util::CRC32::Compute(event_type); }  \
		std::uint32_t GetEventType() const override       \
		    { return GetStaticEventType(); }

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}


}// namespace Helios::Engine
