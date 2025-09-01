#pragma once

#include "Helios/Engine/Events/Event.h"

namespace Helios::Engine {


	class FramebufferResizeEvent : public Event
	{
	public:
		FramebufferResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "FramebufferResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		HE_EVENT_CLASS_TYPE(FramebufferResize)
		HE_EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_Width, m_Height;
	};


	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		HE_EVENT_CLASS_TYPE(WindowResize)
		HE_EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_Width, m_Height;
	};


	class WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(int xpos, int ypos)
			: m_xpos(xpos), m_ypos(ypos) {
		}

		int GetX() const { return m_xpos; }
		int GetY() const { return m_ypos; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << m_xpos << ", " << m_ypos;
			return ss.str();
		}

		HE_EVENT_CLASS_TYPE(WindowMoved)
		HE_EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_xpos, m_ypos;
	};


	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowCloseEvent";
			return ss.str();
		}

		HE_EVENT_CLASS_TYPE(WindowClose)
		HE_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "AppTickEvent";
			return ss.str();
		}

		HE_EVENT_CLASS_TYPE(AppTick)
		HE_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "AppUpdateEvent";
			return ss.str();
		}

		HE_EVENT_CLASS_TYPE(AppUpdate)
		HE_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "AppRenderEvent";
			return ss.str();
		}

		HE_EVENT_CLASS_TYPE(AppRender)
		HE_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


} // namespace Helios::Engine
