//==============================================================================
// Window Management (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file Window.h
//==============================================================================
#include "pch.h"
#include "Helios/Engine/Renderer/Window.h"

#include "Helios/Engine/Renderer/RendererAPI.h"
#include "Helios/Engine/Core/Application.h"
#include "Helios/Engine/Core/EventTypeKey.h"
#include "Helios/Engine/Core/EventTypeMouse.h"
#include "Helios/Engine/Core/EventTypeWindow.h"
#include "Helios/Engine/Core/Config.h"

#ifdef HE_RENDERER_VULKAN
#	include "Helios/Platform/Renderer/Vulkan/VKWindow.h"
#endif
#ifdef HE_RENDERER_DIRECTX
#	include "Helios/Platform/Renderer/DirectX/DXWindow.h"
#endif
#ifdef HE_RENDERER_METAL
#	include "Helios/Platform/Renderer/Metal/MTWindow.h"
#endif

#define CFG ConfigManager::GetInstance()
#define CFG_DOM "window_" + m_WindowName

namespace Helios::Engine::Renderer {


	Ref<Window> Window::Create(const std::string name)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			LOG_RENDER_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;

#		ifdef HE_RENDERER_VULKAN
			case RendererAPI::API::Vulkan: return CreateRef<Vulkan::VKWindow>(name);
#		endif
#		ifdef HE_RENDERER_DIRECTX
			case RendererAPI::API::DirectX: return CreateRef<DirectX::DXWindow>(name);
#		endif
#		ifdef HE_RENDERER_METAL
			case RendererAPI::API::Metal: return CreateRef<Metal::MTWindow>(name);
#		endif
		}

		LOG_RENDER_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	int Window::s_WindowCount = 0;
	Window::Window(const std::string name)
		: m_WindowName(name)
	{
		++s_WindowCount;
		LoadState();
	}


	Window::~Window()
	{
		SaveState();
		--s_WindowCount;
	}


	float Window::GetAspectRatio() const
	{
		if (m_currentState.sizeY == 0)
			return 1.0f;
		return static_cast<float>(m_currentState.sizeX) / static_cast<float>(m_currentState.sizeY);
	}


	void Window::LoadState()
	{
//		std::filesystem::path filepath = Application::Get().GetAppSpec().WorkingDirectory + "/config/window_" + m_WindowName + ".ini";
//		if (!m_Config.Load(filepath))
//			return;
		CFG.LoadDomain(CFG_DOM);


		// Load last pos and size / state
		{
			WindowState& s = m_currentState;
			s.posX  = CFG.Get<int>(CFG_DOM, "Last", "posX",  s.posX);
			s.posY  = CFG.Get<int>(CFG_DOM, "Last", "posY",  s.posY);
			s.sizeX = CFG.Get<int>(CFG_DOM, "Last", "sizeX", s.sizeX);
			s.sizeY = CFG.Get<int>(CFG_DOM, "Last", "sizeY", s.sizeY);

			s.isMaximized  = CFG.Get<bool>(CFG_DOM, "State", "isMaximized",  s.isMaximized);
			s.isMinimized  = CFG.Get<bool>(CFG_DOM, "State", "isMinimized",  s.isMinimized);
			s.isFullscreen = CFG.Get<bool>(CFG_DOM, "State", "isFullscreen", s.isFullscreen);
		}
		
		// Load fullscreen monitor-mode
		{
			WindowMonitor& m = m_monitorState;
			m.monitor = nullptr; // pointer cannot be reconstructed across sessions; DetermineTargetMonitor() will try by name and vidmode
			m.monitorName  = CFG.Get<std::string>(CFG_DOM, "Monitor", "MonitorName", "");
			m.posX         = CFG.Get<int>( CFG_DOM, "Monitor", "posX",         m.posX);
			m.posY         = CFG.Get<int>( CFG_DOM, "Monitor", "posY",         m.posY);
			m.sizeX        = CFG.Get<int>( CFG_DOM, "Monitor", "sizeX",        m.sizeX);
			m.sizeY        = CFG.Get<int>( CFG_DOM, "Monitor", "sizeY",        m.sizeY);
			m.redBits      = CFG.Get<int>( CFG_DOM, "Monitor", "redBits",      m.redBits);
			m.greenBits    = CFG.Get<int>( CFG_DOM, "Monitor", "greenBits",    m.greenBits);
			m.blueBits     = CFG.Get<int>( CFG_DOM, "Monitor", "blueBits",     m.blueBits);
			m.refreshRate  = CFG.Get<int>( CFG_DOM, "Monitor", "refreshRate",  m.refreshRate);
			m.isNonDefault = CFG.Get<bool>(CFG_DOM, "Monitor", "isNonDefault", m.isNonDefault);
		}

		// Load windowd pos and size
		{
			WindowState& s = m_windowedState;
			s.posX  = CFG.Get<int>(CFG_DOM, "Windowed", "posX",  s.posX);
			s.posY  = CFG.Get<int>(CFG_DOM, "Windowed", "posY",  s.posY);
			s.sizeX = CFG.Get<int>(CFG_DOM, "Windowed", "sizeX", s.sizeX);
			s.sizeY = CFG.Get<int>(CFG_DOM, "Windowed", "sizeY", s.sizeY);
		}
	}


	void Window::SaveState()
	{
		// Save current pos and size / state
		CFG.Set<int>(CFG_DOM, "Last", "posX", m_currentState.posX);
		CFG.Set<int>(CFG_DOM, "Last", "posY", m_currentState.posY);
		CFG.Set<int>(CFG_DOM, "Last", "sizeX", m_currentState.sizeX);
		CFG.Set<int>(CFG_DOM, "Last", "sizeY", m_currentState.sizeY);

		CFG.Set<bool>(CFG_DOM, "State", "isMaximized", m_currentState.isMaximized);
		CFG.Set<bool>(CFG_DOM, "State", "isMinimized", m_currentState.isMinimized);
		CFG.Set<bool>(CFG_DOM, "State", "isFullscreen", m_currentState.isFullscreen);
		// Don't save focus state
		// CFG.Set<bool>(CFG_DOM, "State", "isFocused", m_currentState.isFocused);
		// Save fullscreen monitor-mode
		if (!m_monitorState.monitorName.empty()) {
			CFG.Set<std::string>(CFG_DOM, "Monitor", "MonitorName", m_monitorState.monitorName);
			CFG.Set<int>( CFG_DOM, "Monitor", "posX",         m_monitorState.posX);
			CFG.Set<int>( CFG_DOM, "Monitor", "posY",         m_monitorState.posY);
			CFG.Set<int>( CFG_DOM, "Monitor", "sizeX",        m_monitorState.sizeX);
			CFG.Set<int>( CFG_DOM, "Monitor", "sizeY",        m_monitorState.sizeY);
			CFG.Set<int>( CFG_DOM, "Monitor", "redBits",      m_monitorState.redBits);
			CFG.Set<int>( CFG_DOM, "Monitor", "greenBits",    m_monitorState.greenBits);
			CFG.Set<int>( CFG_DOM, "Monitor", "blueBits",     m_monitorState.blueBits);
			CFG.Set<int>( CFG_DOM, "Monitor", "refreshRate",  m_monitorState.refreshRate);
			CFG.Set<bool>(CFG_DOM, "Monitor", "isNonDefault", m_monitorState.isNonDefault);
		}
		else {
			// Clear monitor section if no monitor-mode is set
			// (to avoid stale data)
//			CFG.ClearSection(CFG_DOM, "Monitor");
			CFG.Set<std::string>(CFG_DOM, "Monitor", "MonitorName", "");
		}

		// Save windowed pos and size
		CFG.Set<int>(CFG_DOM, "Windowed", "posX",  m_windowedState.posX);
		CFG.Set<int>(CFG_DOM, "Windowed", "posY",  m_windowedState.posY);
		CFG.Set<int>(CFG_DOM, "Windowed", "sizeX", m_windowedState.sizeX);
		CFG.Set<int>(CFG_DOM, "Windowed", "sizeY", m_windowedState.sizeY);

		CFG.SaveDomain(CFG_DOM);
	}


	GLFWmonitor* Window::DetermineTargetMonitor(bool byWindowCenter)
	{
		int count, index;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		GLFWmonitor* target = nullptr;

		// First try to find the monitor based on name,pos,size + default video mode
		if (!byWindowCenter and !m_monitorState.monitorName.empty()) {
			for (int i = 0; i < count; ++i) {
				// Compare monitor name
				if (glfwGetMonitorName(monitors[i]) == m_monitorState.monitorName) {
					// Compare video mode and position as well
					const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
					int posX = 0, posY = 0;
					glfwGetMonitorPos(monitors[i], &posX, &posY);
					if (mode->width  == m_monitorState.sizeX and
					    mode->height == m_monitorState.sizeY and
					    posX         == m_monitorState.posX and
					    posY         == m_monitorState.posY)
					{
						target = monitors[i];
						index = i;
					}
				}
			}
		}

		// Next, try non-default monitor by name,pos,size + supported video modes
		if (!byWindowCenter and !target) {
			// TODO: implement this
		}
		
		// Next, try to find by window center position
		if (byWindowCenter or !target) {
			// Get window center position
			int posX = 0, posY = 0;
			glfwGetWindowPos(m_Window, &posX, &posY);
			int sizeX = 0, sizeY = 0;
			glfwGetWindowSize(m_Window, &sizeX, &sizeY);
			int cx = posX + sizeX / 2;
			int cy = posY + sizeY / 2;

			// Find monitor that contains the center point
			for (int i = 0; i < count; ++i) {
				const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
				int mx = 0, my = 0;
				glfwGetMonitorPos(monitors[i], &mx, &my);
				if ((mx < cx) and (mx + mode->width > cx) and
					(my < cy) and (my + mode->height > cy))
				{
					target = monitors[i];
					index = i;
				}
			}
		}

		// Finally, default to primary monitor
		if (!target) {
			target = glfwGetPrimaryMonitor();
			index = -1;
		}

		LOG_RENDER_DEBUG("Target monitor: (Idx:{}) {}", index, (index != -1) ? glfwGetMonitorName(target) : "Primary Monitor");
		return target;
	}


	std::tuple<const GLFWvidmode*, bool> Window::DetermineTargetVideoMode(GLFWmonitor* monitor)
	{
		const GLFWvidmode* mode = nullptr;
		bool isNonDefault = false;

		// Try the saved video mode (if set)
		if (m_monitorState.sizeX != -1 and m_monitorState.sizeY != -1 and
		    m_monitorState.sizeX != -1 and m_monitorState.sizeY != -1) {
			int count;
			const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
			for (int i = 0; i < count; ++i) {
				if (modes[i].width       == m_monitorState.sizeX and
				    modes[i].height      == m_monitorState.sizeY and
				    modes[i].redBits     == m_monitorState.redBits and
				    modes[i].greenBits   == m_monitorState.greenBits and
				    modes[i].blueBits    == m_monitorState.blueBits and
				    modes[i].refreshRate == m_monitorState.refreshRate) {
					mode = &modes[i];
					isNonDefault = m_monitorState.isNonDefault;
					break;
				}
			}
		}

		// Fallback to default video mode
		if (!mode) {
			mode = glfwGetVideoMode(monitor);
			isNonDefault = false;
		}

		return std::make_tuple(mode, isNonDefault);
	}


	GLFWmonitor* Window::GetCurrentMonitor()
	{
		GLFWmonitor* monitor = glfwGetWindowMonitor(m_Window);

		// Fullscreen state
		if (monitor) {
			m_monitorState.monitor = monitor;
		}
		// Windowed state -> find monitor by window center
		else {
			// Get window center position
			int posX = 0, posY = 0;
			glfwGetWindowPos(m_Window, &posX, &posY);
			int sizeX = 0, sizeY = 0;
			glfwGetWindowSize(m_Window, &sizeX, &sizeY);
			int cx = posX + sizeX / 2;
			int cy = posY + sizeY / 2;

			// Find monitor that contains the center point
			int count;
			GLFWmonitor** monitors = glfwGetMonitors(&count);
			for (int i = 0; i < count; ++i) {
				const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
				int mx = 0, my = 0;
				glfwGetMonitorPos(monitors[i], &mx, &my);
				if ((mx < cx) and (mx + mode->width > cx) and
					(my < cy) and (my + mode->height > cy))
				{
					monitor = monitors[i];
				}
			}
		}
		
		return monitor;
	}


	const char* Window::GetCurrentMonitorName()
	{
		return glfwGetMonitorName(GetCurrentMonitor());
	}


	void Window::ToggleFullscreen()
	{
		if (!m_Window) return;

		if (glfwIsFullscreen()) {
			// Exit fullscreen
			LOG_CORE_DEBUG("Window: Toggle fullscreen mode (to windowed).");
			m_currentState.isFullscreen = false;
			glfwSetWindowMonitor(m_Window, nullptr,
				m_windowedState.posX,
				m_windowedState.posY,
				m_windowedState.sizeX,
				m_windowedState.sizeY,
				GLFW_DONT_CARE);
			m_monitorState.monitor = GetCurrentMonitor();
		} else {
			// Enter fullscreen
			LOG_CORE_DEBUG("Window: Toggle fullscreen mode (to fullscreen).");

			// depending on m_monitorState.isNonDefault:
			//  true:  try to use saved non-default monitor mode
			//  false: try to use window center position
			GLFWmonitor* target = DetermineTargetMonitor(!m_monitorState.isNonDefault);
			auto [mode, nonDefault] = DetermineTargetVideoMode(target);
			int vx = 0, vy = 0;
			glfwGetMonitorPos(target, &vx, &vy);

			m_currentState.isFullscreen = true;
			glfwSetWindowMonitor(m_Window, target,
				vx, vy, mode->width, mode->height, mode->refreshRate);

			m_monitorState.monitor = target;
			m_monitorState.monitorName = glfwGetMonitorName(target);
			m_monitorState.posX = vx;
			m_monitorState.posY = vy;
			m_monitorState.sizeX = mode->width;
			m_monitorState.sizeY = mode->height;
			m_monitorState.isNonDefault = nonDefault;
		}
	}


	void Window::RaiseEvent(Scope<Event> event)
	{
		OnEvent(*event);
		if (!event->Handled)
			Application::Get().SubmitEvent(std::move(event));
	}


	void Window::OnEvent(Event& e)
	{
		if (e.GetEventType() == EventType::KeyPressed) {
			if (static_cast<KeyPressedEvent&>(e).key == GLFW_KEY_F11) {
				ToggleFullscreen();
				e.Handled = true;
				return;
			}
		}
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowMovedEvent>(HE_BIND_EVENT_FN(Window::OnWindowMoved));
		dispatcher.Dispatch<WindowResizeEvent>(HE_BIND_EVENT_FN(Window::OnWindowResize));
		dispatcher.Dispatch<WindowFocusEvent>(HE_BIND_EVENT_FN(Window::OnWindowFocus));
	}


	bool Window::OnWindowMoved(const WindowMovedEvent& e)
	{
		// Update current state
		m_currentState.posX = e.xpos;
		m_currentState.posY = e.ypos;

		int sizeX = 0, sizeY = 0;
		glfwGetWindowSize(m_Window, &sizeX, &sizeY);
		m_currentState.sizeX = sizeX;
		m_currentState.sizeY = sizeY;

		// Update saved state (windowd) if not fullscreen/minimized/maximized
		if (!glfwIsFullscreen() and !glfwIsMinimized() and !glfwIsMaximized())
			m_windowedState = m_currentState;
		return false;
	}


	bool Window::OnWindowResize(const WindowResizeEvent& e)
	{
		// Update current state
		m_currentState.isFullscreen = glfwIsFullscreen();
		m_currentState.isMinimized  = glfwIsMinimized();
		m_currentState.isMaximized  = glfwIsMaximized();

		glfwGetWindowSize(m_Window, &m_currentState.sizeX, &m_currentState.sizeY);
//		m_currentState.sizeX = e.width;
//		m_currentState.sizeY = e.height;

		int posX = 0, posY = 0;
		glfwGetWindowPos(m_Window, &posX, &posY);
		m_currentState.posX = posX;
		m_currentState.posY = posY;

		// Update saved state (windowd) if not fullscreen/minimized/maximized
		if (!IsFullscreen() and !IsMinimized() and !IsMaximized())
			m_windowedState = m_currentState;

		// Update monitor state if fullscreen
		if (IsFullscreen()) {
			GLFWmonitor* monitor = glfwGetWindowMonitor(m_Window);
			if (monitor) {
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				int posX = 0, posY = 0;
				glfwGetMonitorPos(monitor, &posX, &posY);
				m_monitorState.monitor     = monitor;
				m_monitorState.monitorName = glfwGetMonitorName(monitor);
				m_monitorState.posX        = posX;
				m_monitorState.posY        = posY;
				m_monitorState.sizeX       = mode->width;
				m_monitorState.sizeY       = mode->height;
				m_monitorState.redBits     = mode->redBits;
				m_monitorState.greenBits   = mode->greenBits;
				m_monitorState.blueBits    = mode->blueBits;
				m_monitorState.refreshRate = mode->refreshRate;
			}
		}

		return false;
	}


	bool Window::OnWindowFocus(const WindowFocusEvent& e)
	{
		m_currentState.isFocused = e.isFocused;

		return false;
	}


	void Window::InitCallbacks()
	{
		glfwSetWindowUserPointer(m_Window, this);

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* handle, int xpos, int ypos) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<WindowMovedEvent>(xpos, ypos));
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* handle, int width, int height) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<WindowResizeEvent>(width, height));
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* handle) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<WindowCloseEvent>());
		});

// TODO: Do we need this?
//		glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* handle) {
//			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
//			if (!window) return;
//			window->RaiseEvent(CreateScope<WindowRefreshEvent>());
//		});

		glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* handle, int iconified) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			if (iconified)
				window->RaiseEvent(CreateScope<WindowMinimizeEvent>());
			else
				window->RaiseEvent(CreateScope<WindowRestoreEvent>());
		});

		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* handle, int maximized) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			if (maximized)
				window->RaiseEvent(CreateScope<WindowMaximizeEvent>());
			else
				window->RaiseEvent(CreateScope<WindowRestoreEvent>());
		});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* handle, int focussed) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<WindowFocusEvent>(focussed));
		});

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* handle, int width, int height) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<FramebufferResizeEvent>(width, height));
		});
		
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* handle, int button, int action, int mods)
		{
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			switch (action)
			{
				case GLFW_PRESS:
					window->RaiseEvent(CreateScope<MouseButtonPressedEvent>(button, mods));
					break;
				case GLFW_RELEASE:
					window->RaiseEvent(CreateScope<MouseButtonReleasedEvent>(button, mods));
					break;
			}
		});
		
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* handle, double xPos, double yPos) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<MouseMovedEvent>(static_cast<float>(xPos), static_cast<float>(yPos)));
		});
		
		glfwSetScrollCallback(m_Window, [](GLFWwindow* handle, double xOffset, double yOffset) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<MouseScrolledEvent>(static_cast<float>(xOffset), static_cast<float>(yOffset)));
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* handle, int key, int scancode, int action, int mods)
		{
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			switch (action)
			{
				case GLFW_PRESS:
					window->RaiseEvent(CreateScope<KeyPressedEvent>(key, scancode, 0, mods));
					break;
				case GLFW_RELEASE:
					window->RaiseEvent(CreateScope<KeyReleasedEvent>(key, scancode, mods));
					break;
				case GLFW_REPEAT:
					window->RaiseEvent(CreateScope<KeyPressedEvent>(key, scancode, 1, mods));
					break;
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* handle, unsigned int keycode) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<KeyTypedEvent>(keycode));
		});

		// glfwSetWindowContentScaleCallback
		// glfwSetCursorEnterCallback
		// glfwSetDropCallback
		// glfwSetMonitorCallback
		// glfwSetJoystickCallback
	}


} // namespace Helios::Engine::Renderer
