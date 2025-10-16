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

		virtual GLFWwindow* GetNativeWindow() const = 0;

		virtual void OnUpdate() = 0;
		
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

	private:
		GLFWwindow* m_window = nullptr;
	};


}// namespace Helios::Engine
