#pragma once

#include "Helios/Engine/Events/Event.h"

#include "Helios/Util/IniParser.h"

#include <GLFW/glfw3.h>

namespace Helios::Engine::Renderer {


	class Window
	{
	public:
		static Ref<Window> Create();
		Window();
		~Window();

		GLFWwindow* GetNativeWindow() { return m_Window; }

		virtual void OnUpdate() = 0;
		
		virtual void Show() = 0;
		virtual void Hide() = 0;

//		void ToggleFullscreen();
		void Restore() { if (IsMinimized() or IsMaximized()) glfwRestoreWindow(m_Window); }

		bool IsFullscreen() const { return glfwGetWindowMonitor(m_Window) != nullptr; }
		bool IsMinimized() const { return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) != 0; }
		bool IsMaximized() const { return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED) != 0; }
		bool IsFocused() const { return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) != 0; }

//		void SaveWindowPosition();
//		void LoadWindowPosition();

	protected:
		void LoadConfig();
		void SaveConfig();

		Util::IniParser m_Config;

	protected:
		void RaiseEvent(Scope<Event> event);
		void InitCallbacks();

	protected:
		GLFWwindow* m_Window = nullptr;
	};


} // namespace Helios::Engine::Renderer

// MonitorUtils.h
#if 0
#pragma once

#include <string>
#include <vector>
#include <GLFW/glfw3.h>

namespace Helios::Engine::Renderer {

	struct MonitorVideoMode
	{
		int width;
		int height;
		int redBits;
		int greenBits;
		int blueBits;
		int refreshRate;
	};

	struct MonitorState
	{
		// monitor pointer is valid while GLFW is running; we also keep the monitor name for validation
		GLFWmonitor* monitor = nullptr;
		std::string monitorName;

		// copied video mode (safe to use later)
		MonitorVideoMode vidMode{};

		// windowed restore state
		int windowedPosX = 0;
		int windowedPosY = 0;
		int windowedWidth = 0;
		int windowedHeight = 0;
		bool wasFullscreen = false;
	};

	// Save the current monitor state for the given window.
	// - If the window is fullscreen, the monitor is the window monitor.
	// - If the window is windowed, we pick the monitor that contains the window center (fallback to primary).
	MonitorState SaveMonitorState(GLFWwindow* window) noexcept;

	// Restore previously saved monitor state.
	// - If saved.wasFullscreen is true, this will set the window to fullscreen on the saved monitor.
	// - Otherwise the window is restored to its saved windowed position / size on the saved monitor.
	// - If the saved monitor can no longer be found, the primary monitor is used as fallback.
	void RestoreMonitorState(GLFWwindow* window, MonitorState const& saved) noexcept;

} // namespace Helios::Engine::Renderer
#endif

// MonitorUtils.cpp
#if 0
#include "MonitorUtils.h"

namespace Helios::Engine::Renderer {

	static MonitorVideoMode CopyVidMode(const GLFWvidmode* vm) noexcept
	{
		MonitorVideoMode m{};
		if (!vm) return m;
		m.width = vm->width;
		m.height = vm->height;
		m.redBits = vm->redBits;
		m.greenBits = vm->greenBits;
		m.blueBits = vm->blueBits;
		m.refreshRate = vm->refreshRate;
		return m;
	}

	static GLFWmonitor* FindMonitorContainingPoint(int x, int y) noexcept
	{
		int count = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		if (!monitors || count == 0) return glfwGetPrimaryMonitor();

		for (int i = 0; i < count; ++i) {
			GLFWmonitor* m = monitors[i];
			int wx, wy, wwidth, wheight;
#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3
			// work area gives usable area (taskbar etc). If unavailable, fallback to monitor pos + mode size.
			glfwGetMonitorWorkarea(m, &wx, &wy, &wwidth, &wheight);
#else
			glfwGetMonitorPos(m, &wx, &wy);
			const GLFWvidmode* vm = glfwGetVideoMode(m);
			wwidth = vm ? vm->width : 0;
			wheight = vm ? vm->height : 0;
#endif
			if (x >= wx && x < wx + wwidth && y >= wy && y < wy + wheight)
				return m;
		}
		return glfwGetPrimaryMonitor();
	}

	MonitorState SaveMonitorState(GLFWwindow* window) noexcept
	{
		MonitorState s{};

		if (!window) return s;

		GLFWmonitor* winMon = glfwGetWindowMonitor(window);
		s.wasFullscreen = (winMon != nullptr);

		// Save windowed geometry always so we can restore later
		int wx = 0, wy = 0, ww = 0, wh = 0;
		glfwGetWindowPos(window, &wx, &wy);
		glfwGetWindowSize(window, &ww, &wh);
		s.windowedPosX = wx;
		s.windowedPosY = wy;
		s.windowedWidth = ww;
		s.windowedHeight = wh;

		// Determine monitor: if fullscreen window has a monitor, use it; otherwise pick monitor under window center
		GLFWmonitor* chosen = winMon;
		if (!chosen) {
			int centerX = wx + (ww / 2);
			int centerY = wy + (wh / 2);
			chosen = FindMonitorContainingPoint(centerX, centerY);
		}

		s.monitor = chosen;
		if (chosen) {
			const char* name = glfwGetMonitorName(chosen);
			if (name) s.monitorName = name;
			const GLFWvidmode* vm = glfwGetVideoMode(chosen);
			s.vidMode = CopyVidMode(vm);
		}

		return s;
	}

	void RestoreMonitorState(GLFWwindow* window, MonitorState const& saved) noexcept
	{
		if (!window) return;

		// Attempt to re-find monitor by name (handles monitors reordering)
		GLFWmonitor* target = nullptr;
		if (!saved.monitorName.empty()) {
			int count = 0;
			GLFWmonitor** monitors = glfwGetMonitors(&count);
			for (int i = 0; monitors && i < count; ++i) {
				const char* name = glfwGetMonitorName(monitors[i]);
				if (name && saved.monitorName == name) {
					target = monitors[i];
					break;
				}
			}
		}

		// Fallbacks
		if (!target) {
			// if saved.monitor still valid (pointer may still be ok during runtime), try it
			if (saved.monitor) target = saved.monitor;
		}
		if (!target) target = glfwGetPrimaryMonitor();

		// If the saved state indicated fullscreen, restore fullscreen on the chosen monitor
		if (saved.wasFullscreen) {
			int vx = 0, vy = 0;
#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3
			// place at monitor position
			glfwGetMonitorPos(target, &vx, &vy);
#endif
			int width = saved.vidMode.width ? saved.vidMode.width : saved.windowedWidth;
			int height = saved.vidMode.height ? saved.vidMode.height : saved.windowedHeight;
			int refresh = saved.vidMode.refreshRate;
			glfwSetWindowMonitor(window, target, vx, vy, width, height, refresh);
		}
		else {
			// Windowed: set windowed pos/size on the target monitor.
			// Choose a reasonable position: saved windowed pos if it still lies inside target monitor, otherwise center on target.
			int cx = saved.windowedPosX + (saved.windowedWidth / 2);
			int cy = saved.windowedPosY + (saved.windowedHeight / 2);

			bool insideTarget = false;
			int mx = 0, my = 0, mw = 0, mh = 0;
#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3
			glfwGetMonitorWorkarea(target, &mx, &my, &mw, &mh);
#else
			glfwGetMonitorPos(target, &mx, &my);
			const GLFWvidmode* vm = glfwGetVideoMode(target);
			mw = vm ? vm->width : 0;
			mh = vm ? vm->height : 0;
#endif
			if (cx >= mx && cx < mx + mw && cy >= my && cy < my + mh)
				insideTarget = true;

			if (insideTarget) {
				glfwSetWindowMonitor(window, nullptr, saved.windowedPosX, saved.windowedPosY,
					saved.windowedWidth, saved.windowedHeight, saved.vidMode.refreshRate);
			}
			else {
				// center on target
				int px = mx + (mw - saved.windowedWidth) / 2;
				int py = my + (mh - saved.windowedHeight) / 2;
				glfwSetWindowMonitor(window, nullptr, px, py, saved.windowedWidth, saved.windowedHeight, saved.vidMode.refreshRate);
			}
		}
	}

} // namespace Helios::Engine::Renderer
#endif
