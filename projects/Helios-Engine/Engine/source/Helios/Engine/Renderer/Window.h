#pragma once

#include "Helios/Engine/Renderer/RendererSpec.h"
#include "Helios/Engine/Events/Event.h"

namespace Helios::Engine {


	class Window
	{
	public:

		static Scope<Window> Create(RendererSpec& spec);
		Window(RendererSpec& spec);
		~Window();

		void OnUpdate();

//		unsigned int GetWidth() { return m_Data.Width; }
//		unsigned int GetHeight() { return m_Data.Height; }

		using EventCallbackFn = std::function<void(Event&)>;
		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

//		void SetVSync(bool enabled);
//		bool IsVSync() const;

		void* GetNativeWindow() const { return m_Window; }

	private:

		void InitCallbacks();

	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			EventCallbackFn EventCallback;
		};
		WindowData m_Data;
	};


} // end of namespace Helios::Engine
