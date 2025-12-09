#pragma once

#include "Helios/Engine/Events/Event.h"

#include <GLFW/glfw3.h>

namespace Helios::Engine::Renderer {


	class Window
	{
	public:
		static Ref<Window> Create();
		virtual ~Window() = default;

		GLFWwindow* GetNativeWindow() { return m_Window; }

		virtual void OnUpdate() = 0;
		
		virtual void Show() = 0;
		virtual void Hide() = 0;

	protected:
		void RaiseEvent(Scope<Event> event);
		void InitCallbacks();

	protected:
		GLFWwindow* m_Window = nullptr;
	};


} // namespace Helios::Engine::Renderer
