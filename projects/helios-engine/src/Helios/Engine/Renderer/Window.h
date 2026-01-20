#pragma once

#include "Helios/Engine/Core/Events.h"
//#include "Helios/Engine/Renderer/MonitorUtils.h"
#include "Helios/Engine/Util/IniParser.h"

namespace Helios::Engine::Renderer {


	struct WindowMonitor
	{
		GLFWmonitor* monitor = nullptr;
		std::string monitorName{};

		// Position and size of the monitor's video mode
		int  posX         = -1;
		int  posY         = -1;
		int  sizeX        = -1;
		int  sizeY        = -1;
		// Color bits and refresh rate
		int  redBits      = 0;
		int  greenBits    = 0;
		int  blueBits     = 0;
		int  refreshRate  = 0;
		// Whether this monitor-mode is non-default (i.e., not the default/desktop video-mode)
		bool isNonDefault = false;
	};
	struct WindowState {
		// Position: (-1,-1 means "undefined" (let the OS decide))
		int posX  = -1;
		int posY  = -1;
		int sizeX = 800;
		int sizeY = 600;
		// Window state flags (only used in currentState)
		bool isFullscreen = false;
		bool isMaximized  = false;
		bool isMinimized  = false;
		bool isFocused    = true;
	};


	class Window
	{
	public:
		static Ref<Window> Create(const std::string name = "app");
		Window(const std::string name);
		~Window();

		GLFWwindow* GetNativeWindow() { return m_Window; }

		virtual void Show() { glfwShowWindow(m_Window); }
		virtual void Hide() { glfwHideWindow(m_Window); }

		void ToggleFullscreen();
		void Restore() { if (IsMinimized() or IsMaximized()) glfwRestoreWindow(m_Window); }

		bool IsFullscreen() const { return m_currentState.isFullscreen; }
		bool IsMinimized() const { return m_currentState.isMinimized; }
		bool IsMaximized() const { return m_currentState.isMaximized; }
		bool IsFocused() const { return m_currentState.isFocused; }

		bool glfwIsFullscreen() const { return glfwGetWindowMonitor(m_Window) != nullptr; }
		bool glfwIsMinimized() const { return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) != 0; }
		bool glfwIsMaximized() const { return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED) != 0; }
		bool glfwIsFocused() const { return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) != 0; }

	protected:
		void LoadState();
		void SaveState();
		GLFWmonitor* DetermineTargetMonitor(bool byWindowCenter = false);
		std::tuple<const GLFWvidmode*, bool> DetermineTargetVideoMode(GLFWmonitor* monitor);
		GLFWmonitor* GetCurrentMonitor();
		const char* GetCurrentMonitorName();

		Util::IniParser m_Config;

	protected:
		void InitCallbacks();
		void RaiseEvent(Scope<Event> event);
		void OnEvent(Event& e);
		bool OnWindowMoved(const WindowMovedEvent& e);
		bool OnWindowResize(const WindowResizeEvent& e);
		bool OnWindowFocus(const WindowFocusEvent& e);

	protected:
		std::string m_WindowName;
		static int s_WindowCount;
		GLFWwindow* m_Window = nullptr;

		WindowState m_currentState; // current window pos/size
		WindowState m_windowedState; // windowed mode pos/size (saved to restore from fullscreen)
		WindowMonitor m_monitorState; // monitor state for fullscreen mode
	};


} // namespace Helios::Engine::Renderer
