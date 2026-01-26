//==============================================================================
// Event System
//
// Provides a polymorphic event handling framework with type-safe dispatching.
// Supports multiple events types and event categories for filtering.
// Uses the EventDispatcher for type-safe callback invocation with automatic
// event type matching and handling state management.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - Type-safe polymorphic event dispatching
// - Event category filtering with bitfield flags
// - Automatic handling state tracking
// - Comprehensive window and input event types
// - Modifier key support
// - Macro-based event class generation
// - Lambda callback binding
// - Lambda binding helper (HE_BIND_EVENT_FN)
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

namespace Helios::Engine {


	enum class EventType : std::uint32_t
	{
		None = 0,

		WindowClose,
		WindowResize,
		FramebufferResize,
		WindowFocus,
		WindowMoved,
		WindowMinimize,
		WindowMaximize,
		WindowRestore,
		WindowRefresh,
		
//		AppTick,
//		AppUpdate,
//		AppRender,
		
		KeyPressed,
		KeyReleased,
		KeyTyped,
		
		MouseMoved,
		MouseScrolled,
		MouseButtonPressed,
		MouseButtonReleased,
	};

	enum EventCategory : std::uint32_t
	{
		None        = 0,

		Window      = 1 << 0,
		Input       = 1 << 1,
		Keyboard    = 1 << 2,
		Mouse       = 1 << 3,
		MouseButton = 1 << 4,
	};

	// for key and mouse button modifier flags
	enum class KeyMods : std::uint16_t
	{
		None     = 0,

		Shift    = 1 << 0,
		Control  = 1 << 1,
		Alt      = 1 << 2,
		Super    = 1 << 3,

		CapsLock = 1 << 4,
		NumLock  = 1 << 5,
	};

	class Event
	{
	public:
		bool Handled = false;

		virtual ~Event() = default;
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
		virtual int GetCategoryFlags() const = 0;
		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & static_cast<int>(category);
		}
	};

	#define EVENT_CLASS_TYPE(type) \
		static EventType GetEventTypeStatic() { return EventType::type; } \
		virtual EventType GetEventType() const override { return GetEventTypeStatic(); } \
		virtual const char* GetName() const override { return #type"Event"; }
	#define EVENT_CLASS_CATEGORY(category) \
		virtual int GetCategoryFlags() const { return category; }


	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(const T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		template<typename T>
		bool Dispatch(const EventFn<T>& func)
		{
			if (m_Event.GetEventType() == T::GetEventTypeStatic() && !m_Event.Handled)
			{
				m_Event.Handled = func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	#define HE_BIND_EVENT_FN(fn) \
		[this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }


} // namespace Helios::Engine
