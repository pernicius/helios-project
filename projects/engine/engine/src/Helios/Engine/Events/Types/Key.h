#pragma once

#include "Helios/Engine/Events/Event.h"

using KeyCode = uint16_t;

namespace Helios::Engine
{


	class KeyPressedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(KeyPressed);
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Keyboard);

		KeyPressedEvent(int keyCode, int repeatCount)
			: key(keyCode)
			, repeatCount(repeatCount)
		{ }

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
		EVENT_CLASS_TYPE(KeyReleased);
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Keyboard);

		KeyReleasedEvent(int keyCode)
			: key(keyCode)
		{ }

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
		EVENT_CLASS_TYPE(KeyTyped);
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Keyboard);

		KeyTypedEvent(unsigned int keyCode)
			: key(keyCode)
		{ }

		std::string ToString() const override
		{
			return fmt::format("KeyTypedEvent: {}", key);
		}

	public:
		KeyCode key{ 0 };
	};


}// namespace Helios::Engine
