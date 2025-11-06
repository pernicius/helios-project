#pragma once
#include "Helios/Engine/Events/Event.h"

namespace Helios::Engine::Events
{


	class WindowCloseEvent : public Event
	{
	public:
		EVENT_TYPE("WindowCloseEvent");
	};


	class WindowResizeEvent : public Event
	{
	public:
		EVENT_TYPE("WindowResizeEvent");

		WindowResizeEvent(unsigned int width_, unsigned int height_)
		: width(width_)
		, height(height_)
		{
		}

		std::string ToString() const override
		{
			return fmt::format("WindowResizeEvent: {}, {}", width, height);
		}

	public:
		unsigned int width{ 0 };
		unsigned int height{ 0 };
	};


}// namespace Helios::Engine::Events
