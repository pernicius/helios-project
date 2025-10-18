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

		GLFWwindow* GetNativeWindow() { return m_window; }

		virtual void OnUpdate() = 0;
		
		unsigned int GetWidth() { return m_width; }
		unsigned int GetHeight() { return m_height; }

	private:
		GLFWwindow* m_window = nullptr;
		unsigned int m_width = 0;
		unsigned int m_height = 0;
	};


}// namespace Helios::Engine
