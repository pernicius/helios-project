#pragma once

#include "Helios/Engine/Events/Event.h"

using MouseCode = uint16_t;

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

		MouseButtonPressedEvent(int button)
		: button(button)
		{ }

		std::string ToString() const override
		{
			return fmt::format("MouseButtonPressedEvent: {}", button);
		}

	public:
		MouseCode button{ 0 };
	};


	class MouseButtonReleasedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(MouseButtonReleased);
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::MouseButton);

		MouseButtonReleasedEvent(int button)
		: button(button)
		{ }

		std::string ToString() const override
		{
			return fmt::format("MouseButtonReleasedEvent: {}", button);
		}

	public:
		MouseCode button{ 0 };
	};


}// namespace Helios::Engine
