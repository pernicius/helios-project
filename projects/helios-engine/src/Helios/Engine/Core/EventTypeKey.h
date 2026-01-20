#pragma once
#include "Helios/Engine/Core/Events.h"

using KeyCode = uint16_t;
using ModKeys = uint16_t;

namespace Helios::Engine
{


	class KeyPressedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(KeyPressed);
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Keyboard);

		KeyPressedEvent(int keyCode, int scancode, int repeatCount, int mods)
			: key(keyCode)
			, scancode(scancode)
			, repeatCount(repeatCount)
			, mods(mods)
		{ }

		std::string ToString() const override
		{
			return fmt::format("KeyPressedEvent: {} ({} repeats)", key, repeatCount);
		}

		const char* GetKeyName() const
		{
			return glfwGetKeyName(key, scancode);
		}

	public:
		KeyCode key{ 0 };
		int scancode{ 0 };
		int repeatCount{ 0 };
		ModKeys mods{ 0 };
	};


	class KeyReleasedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(KeyReleased);
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Keyboard);

		KeyReleasedEvent(int keyCode, int scancode, int mods)
			: key(keyCode)
			, scancode(scancode)
			, mods(mods)
		{ }

		std::string ToString() const override
		{
			return fmt::format("KeyReleasedEvent: {}", key);
		}

		const char* GetKeyName() const
		{
			return glfwGetKeyName(key, scancode);
		}

	public:
		KeyCode key{ 0 };
		int scancode{ 0 };
		ModKeys mods{ 0 };
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

//		std::string ToUTF8()
//		{
// 		   TODO: implement conversion from codepoint to UTF-8 string
//		}

	public:
		unsigned int key{ 0 };
	};


}// namespace Helios::Engine
