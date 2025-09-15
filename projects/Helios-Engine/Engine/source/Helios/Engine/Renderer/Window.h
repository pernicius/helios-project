#pragma once

#include "Helios/Engine/Renderer/Renderer.h"
#include "Helios/Engine/Events/Event.h"

namespace Helios::Engine {


	class Window
	{
	public:

		static Ref<Window> Create(Renderer::Specification& spec);
		Window() = delete;
		Window(Renderer::Specification& spec);
		~Window();

		void* GetNativeWindow() const { return m_Window; }

		void Show();

		void OnUpdate();

		using EventCallbackFn = std::function<void(Event&)>;
		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

	private:
		void InitCallbacks();

	private:
		Renderer::Specification& m_Spec;
		GLFWwindow* m_Window;

		struct WindowData
		{
			EventCallbackFn EventCallback;
			int test;
		};
		WindowData m_Data;
	};


} // end of namespace Helios::Engine
