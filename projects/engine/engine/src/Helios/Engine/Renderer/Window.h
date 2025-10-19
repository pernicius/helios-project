#pragma once

#include <Helios/Util/ScopeRef.h>

#include <GLFW/glfw3.h>

#include <string>

namespace Helios::Engine
{


	class Window
	{
	public:
		static Scope<Window> Create();
		virtual ~Window() = default;

		GLFWwindow* GetNativeWindow() { return m_Window; }

		virtual void OnUpdate() = 0;
		
		virtual void Show() = 0;
		virtual void Hide() = 0;

	protected:
		GLFWwindow* m_Window = nullptr;
	};


}// namespace Helios::Engine
