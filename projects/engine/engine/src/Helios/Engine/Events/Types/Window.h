#pragma once

#include "Helios/Engine/Events/Event.h"

namespace Helios::Engine
{


	class WindowCloseEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowClose);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		WindowCloseEvent() = default;
	};


	class WindowResizeEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowResize);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		WindowResizeEvent(int width_, int height_)
		: width(width_)
		, height(height_)
		{ }

		std::string ToString() const override
		{
			return fmt::format("{}: {}, {}", GetName(), width, height);
		}

	public:
		int width{ 0 };
		int height{ 0 };
	};


	class FramebufferResizeEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(FramebufferResize);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		FramebufferResizeEvent(int width_, int height_)
		: width(width_)
		, height(height_)
		{ }

		std::string ToString() const override
		{
			return fmt::format("{}: {}, {}", GetName(), width, height);
		}

	public:
		int width{ 0 };
		int height{ 0 };
	};


}// namespace Helios::Engine
