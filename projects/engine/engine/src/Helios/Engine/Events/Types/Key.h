#pragma once
#include "Helios/Engine/Events/Event.h"

using KeyCode = uint16_t;

namespace Helios::Engine::Events
{


	class KeyPressedEvent : public Event
	{
	public:
		EVENT_TYPE("KeyPressedEvent");

		KeyPressedEvent(int keyCode, int repeatCount)
			: key(keyCode)
			, repeatCount(repeatCount)
		{
		}

		std::string ToString() const override
		{
			return fmt::format("KeyPressedEvent: {} ({} repeats)", key, repeatCount);
		}

	public:
		KeyCode key{ 0 };
		int repeatCount{ 0 };
	};


	class KeyReleasedEvent : public Event
	{
	public:
		EVENT_TYPE("KeyReleasedEvent");

		KeyReleasedEvent(int keyCode)
			: key(keyCode)
		{
		}

		std::string ToString() const override
		{
			return fmt::format("KeyReleasedEvent: {}", key);
		}

	public:
		KeyCode key{ 0 };
	};


	class KeyTypedEvent : public Event
	{
	public:
		EVENT_TYPE("KeyTypedEvent");

		KeyTypedEvent(int keyCode)
			: key(keyCode)
		{
		}

		std::string ToString() const override
		{
			return fmt::format("KeyTypedEvent: {}", key);
		}

	public:
		KeyCode key{ 0 };
	};


}// namespace Helios::Engine::Events
