//==============================================================================
// Mouse Event Types
//
// Defines concrete mouse event classes derived from the Event base class.
// Includes MouseMovedEvent (cursor position), MouseScrolledEvent (wheel delta),
// MouseButtonPressedEvent, and MouseButtonReleasedEvent (with modifier keys).
// Uses fmt for string formatting. Part of the engine's event system for
// mouse input handling.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Changelog:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once
#include "Helios/Engine/Core/Events.h"

using MouseCode = uint16_t;
using ModKeys = uint16_t;

namespace Helios::Engine
{


	class MouseMovedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(MouseMoved);
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Mouse);

		MouseMovedEvent(float x, float y)
		: mouseX(x)
		, mouseY(y)
		{ }

		std::string ToString() const override
		{
			return fmt::format("MouseMovedEvent: {}, {}", mouseX, mouseY);
		}

	public:
		float mouseX{ 0.0f };
		float mouseY{ 0.0f };
	};


	class MouseScrolledEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(MouseScrolled);
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Mouse);

		MouseScrolledEvent(float xOffset_, float yOffset_)
		: xOffset(xOffset_)
		, yOffset(yOffset_)
		{ }

		std::string ToString() const override
		{
			return fmt::format("MouseScrolledEvent: {}, {}", xOffset, yOffset);
		}

	public:
		float xOffset{ 0.0f };
		float yOffset{ 0.0f };
	};


	class MouseButtonPressedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(MouseButtonPressed);
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::MouseButton);

		MouseButtonPressedEvent(int button, int mods)
			: button(button), mods(mods)
		{ }

		std::string ToString() const override
		{
			return fmt::format("MouseButtonPressedEvent: {}", button);
		}

	public:
		MouseCode button{ 0 };
		ModKeys mods{ 0 };
	};


	class MouseButtonReleasedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(MouseButtonReleased);
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::MouseButton);

		MouseButtonReleasedEvent(int button, int mods)
		: button(button), mods(mods)
		{ }

		std::string ToString() const override
		{
			return fmt::format("MouseButtonReleasedEvent: {}", button);
		}

	public:
		MouseCode button{ 0 };
		ModKeys mods{ 0 };
	};


}// namespace Helios::Engine
