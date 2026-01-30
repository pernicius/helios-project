//==============================================================================
// Window Event Types
//
// Defines concrete window event classes derived from the Event base class.
// Includes lifecycle events (WindowClose, WindowMinimize, WindowMaximize,
// WindowRestore, WindowRefresh), state events (WindowFocus with focus state),
// position events (WindowMoved), and size events (WindowResize,
// FramebufferResize). Uses fmt for string formatting. Part of the engine's
// event system for window management.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Changelog:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once
#include "Helios/Engine/Core/Events.h"

namespace Helios::Engine
{


	class WindowCloseEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowClose);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		WindowCloseEvent() = default;
	};


	class WindowMinimizeEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowMinimize);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		WindowMinimizeEvent() = default;
	};


	class WindowMaximizeEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowMaximize);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		WindowMaximizeEvent() = default;
	};


	class WindowRestoreEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowRestore);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		WindowRestoreEvent() = default;
	};


	class WindowRefreshEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowRefresh);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		WindowRefreshEvent() = default;
	};


	class WindowFocusEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowFocus);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		WindowFocusEvent(bool isFocused_)
			: isFocused(isFocused_)
		{
		}

		std::string ToString() const override
		{
			return fmt::format("{}: {}", GetName(), isFocused ? "got focus" : "lost focus");
		}

	public:
		bool isFocused{ false };
	};


	class WindowMovedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowMoved);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		WindowMovedEvent(int xpos_, int ypos_)
			: xpos(xpos_)
			, ypos(ypos_)
		{
		}

		std::string ToString() const override
		{
			return fmt::format("{}: {}, {}", GetName(), xpos, ypos);
		}

	public:
		int xpos{ 0 };
		int ypos{ 0 };
	};


	class WindowResizeEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowResize);
		EVENT_CLASS_CATEGORY(EventCategory::Window);

		WindowResizeEvent(int width_, int height_)
			: width(width_)
			, height(height_)
		{
		}

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


} // namespace Helios::Engine
