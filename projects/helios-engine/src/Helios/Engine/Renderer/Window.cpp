#include "pch.h"
#include "Helios/Engine/Renderer/Window.h"

#include "Helios/Engine/Renderer/RendererAPI.h"
#include "Helios/Engine/Core/Application.h"
#include "Helios/Engine/Core/EventTypeKey.h"
#include "Helios/Engine/Core/EventTypeMouse.h"
#include "Helios/Engine/Core/EventTypeWindow.h"

#ifdef HE_RENDERER_VULKAN
#	include "Helios/Platform/Renderer/Vulkan/VKWindow.h"
#endif
#ifdef HE_RENDERER_DIRECTX
#	include "Helios/Platform/Renderer/DirectX/DX12Window.h"
#endif
#ifdef HE_RENDERER_METAL
#	include "Helios/Platform/Renderer/Metal/MTWindow.h"
#endif

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


	void Window::LoadState()
	{
		std::filesystem::path filepath = Application::Get().GetAppSpec().WorkingDirectory + "/config/window_" + m_WindowName + ".ini";
		if (!m_Config.Load(filepath))
			return;

		// Load last pos and size / state
		{
			WindowState& s = m_currentState;
			s.posX  = m_Config.Get<int>("Last", "posX",  s.posX);
			s.posY  = m_Config.Get<int>("Last", "posY",  s.posY);
			s.sizeX = m_Config.Get<int>("Last", "sizeX", s.sizeX);
			s.sizeY = m_Config.Get<int>("Last", "sizeY", s.sizeY);

			s.isMaximized  = m_Config.Get<bool>("State", "isMaximized",  s.isMaximized);
			s.isMinimized  = m_Config.Get<bool>("State", "isMinimized",  s.isMinimized);
			s.isFullscreen = m_Config.Get<bool>("State", "isFullscreen", s.isFullscreen);
		}
		
		// Load fullscreen monitor-mode
		{
			WindowMonitor& m = m_monitorState;
			m.monitor = nullptr; // pointer cannot be reconstructed across sessions; DetermineTargetMonitor() will try by name and vidmode
			m.monitorName  = m_Config.Get<std::string>("Monitor", "MonitorName", "");
			m.posX         = m_Config.Get<int>( "Monitor", "posX",         m.posX);
			m.posY         = m_Config.Get<int>( "Monitor", "posY",         m.posY);
			m.sizeX        = m_Config.Get<int>( "Monitor", "sizeX",        m.sizeX);
			m.sizeY        = m_Config.Get<int>( "Monitor", "sizeY",        m.sizeY);
			m.redBits      = m_Config.Get<int>( "Monitor", "redBits",      m.redBits);
			m.greenBits    = m_Config.Get<int>( "Monitor", "greenBits",    m.greenBits);
			m.blueBits     = m_Config.Get<int>( "Monitor", "blueBits",     m.blueBits);
			m.refreshRate  = m_Config.Get<int>( "Monitor", "refreshRate",  m.refreshRate);
			m.isNonDefault = m_Config.Get<bool>("Monitor", "isNonDefault", m.isNonDefault);
		}

		// Load windowd pos and size
		{
			WindowState& s = m_windowedState;
			s.posX  = m_Config.Get<int>("Windowed", "posX",  s.posX);
			s.posY  = m_Config.Get<int>("Windowed", "posY",  s.posY);
			s.sizeX = m_Config.Get<int>("Windowed", "sizeX", s.sizeX);
			s.sizeY = m_Config.Get<int>("Windowed", "sizeY", s.sizeY);
		}
	}


	void Window::SaveState()
	{
		std::filesystem::path filepath = Application::Get().GetAppSpec().WorkingDirectory + "/config/window_" + m_WindowName + ".ini";

		// Save current pos and size / state
		m_Config.Set<int>("Last", "posX", m_currentState.posX);
		m_Config.Set<int>("Last", "posY", m_currentState.posY);
		m_Config.Set<int>("Last", "sizeX", m_currentState.sizeX);
		m_Config.Set<int>("Last", "sizeY", m_currentState.sizeY);

		m_Config.Set<bool>("State", "isMaximized", m_currentState.isMaximized);
		m_Config.Set<bool>("State", "isMinimized", m_currentState.isMinimized);
		m_Config.Set<bool>("State", "isFullscreen", m_currentState.isFullscreen);
		// Don't save focus state
		// m_Config.Set<bool>("State", "isFocused", m_currentState.isFocused);

		// Save fullscreen monitor-mode
		if (!m_monitorState.monitorName.empty()) {
			m_Config.Set<std::string>("Monitor", "MonitorName", m_monitorState.monitorName);
			m_Config.Set<int>( "Monitor", "posX",         m_monitorState.posX);
			m_Config.Set<int>( "Monitor", "posY",         m_monitorState.posY);
			m_Config.Set<int>( "Monitor", "sizeX",        m_monitorState.sizeX);
			m_Config.Set<int>( "Monitor", "sizeY",        m_monitorState.sizeY);
			m_Config.Set<int>( "Monitor", "redBits",      m_monitorState.redBits);
			m_Config.Set<int>( "Monitor", "greenBits",    m_monitorState.greenBits);
			m_Config.Set<int>( "Monitor", "blueBits",     m_monitorState.blueBits);
			m_Config.Set<int>( "Monitor", "refreshRate",  m_monitorState.refreshRate);
			m_Config.Set<bool>("Monitor", "isNonDefault", m_monitorState.isNonDefault);
		}
		else {
			// Clear monitor section if no monitor-mode is set
			// (to avoid stale data)
			m_Config.ClearSection("Monitor");
		}

		// Save windowed pos and size
		m_Config.Set<int>("Windowed", "posX",  m_windowedState.posX);
		m_Config.Set<int>("Windowed", "posY",  m_windowedState.posY);
		m_Config.Set<int>("Windowed", "sizeX", m_windowedState.sizeX);
		m_Config.Set<int>("Windowed", "sizeY", m_windowedState.sizeY);

		m_Config.Save(filepath);
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

		m_currentState.sizeX = e.width;
		m_currentState.sizeY = e.height;

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

		glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* handle) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<WindowRefreshEvent>());
		});

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
