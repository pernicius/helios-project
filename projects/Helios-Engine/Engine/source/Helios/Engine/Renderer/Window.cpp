#include "pch.h"
#include "Helios/Engine/Renderer/Window.h"

#include "Helios/Engine/Events/ApplicationEvent.h"
#include "Helios/Engine/Events/MouseEvent.h"
#include "Helios/Engine/Events/KeyEvent.h"

#ifdef TARGET_PLATFORM_WINDOWS
#	include <ShellScalingApi.h>
#	pragma comment(lib, "shcore.lib")
#endif

namespace Helios::Engine {


	Scope<Window> Window::Create(RendererSpec& spec)
	{
		LOG_CORE_DEBUG("Creating main window...");
		return CreateScope<Window>(spec);
	}
	static uint8_t s_GLFWWindowCount = 0;


	Window::Window(RendererSpec& spec)
	{
#		ifdef TARGET_PLATFORM_WINDOWS
			if (!spec.Window.enablePerMonitorDPI)
			{
				// glfwInit enables the maximum supported level of DPI awareness unconditionally.
				// If the app doesn't need it, we have to call this function before glfwInit to override that behavior.
				SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
			}
#		endif

		int success = glfwInit();
		LOG_GLFW_ASSERT(success, "Could not initialize GLFW!");

		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		const struct
		{
			nvrhi::Format format;
			uint32_t redBits;
			uint32_t greenBits;
			uint32_t blueBits;
			uint32_t alphaBits;
			uint32_t depthBits;
			uint32_t stencilBits;
		} formatInfo[] = {
			{ nvrhi::Format::UNKNOWN,            0,  0,  0,  0,  0,  0, },
			{ nvrhi::Format::R8_UINT,            8,  0,  0,  0,  0,  0, },
			{ nvrhi::Format::RG8_UINT,           8,  8,  0,  0,  0,  0, },
			{ nvrhi::Format::RG8_UNORM,          8,  8,  0,  0,  0,  0, },
			{ nvrhi::Format::R16_UINT,          16,  0,  0,  0,  0,  0, },
			{ nvrhi::Format::R16_UNORM,         16,  0,  0,  0,  0,  0, },
			{ nvrhi::Format::R16_FLOAT,         16,  0,  0,  0,  0,  0, },
			{ nvrhi::Format::RGBA8_UNORM,        8,  8,  8,  8,  0,  0, },
			{ nvrhi::Format::RGBA8_SNORM,        8,  8,  8,  8,  0,  0, },
			{ nvrhi::Format::BGRA8_UNORM,        8,  8,  8,  8,  0,  0, },
			{ nvrhi::Format::SRGBA8_UNORM,       8,  8,  8,  8,  0,  0, },
			{ nvrhi::Format::SBGRA8_UNORM,       8,  8,  8,  8,  0,  0, },
			{ nvrhi::Format::R10G10B10A2_UNORM, 10, 10, 10,  2,  0,  0, },
			{ nvrhi::Format::R11G11B10_FLOAT,   11, 11, 10,  0,  0,  0, },
			{ nvrhi::Format::RG16_UINT,         16, 16,  0,  0,  0,  0, },
			{ nvrhi::Format::RG16_FLOAT,        16, 16,  0,  0,  0,  0, },
			{ nvrhi::Format::R32_UINT,          32,  0,  0,  0,  0,  0, },
			{ nvrhi::Format::R32_FLOAT,         32,  0,  0,  0,  0,  0, },
			{ nvrhi::Format::RGBA16_FLOAT,      16, 16, 16, 16,  0,  0, },
			{ nvrhi::Format::RGBA16_UNORM,      16, 16, 16, 16,  0,  0, },
			{ nvrhi::Format::RGBA16_SNORM,      16, 16, 16, 16,  0,  0, },
			{ nvrhi::Format::RG32_UINT,         32, 32,  0,  0,  0,  0, },
			{ nvrhi::Format::RG32_FLOAT,        32, 32,  0,  0,  0,  0, },
			{ nvrhi::Format::RGB32_UINT,        32, 32, 32,  0,  0,  0, },
			{ nvrhi::Format::RGB32_FLOAT,       32, 32, 32,  0,  0,  0, },
			{ nvrhi::Format::RGBA32_UINT,       32, 32, 32, 32,  0,  0, },
			{ nvrhi::Format::RGBA32_FLOAT,      32, 32, 32, 32,  0,  0, },
		};
		bool foundFormat = false;
		for (const auto& info : formatInfo)
		{
			if (info.format == spec.Device.swapChainFormat)
			{
				glfwWindowHint(GLFW_RED_BITS, info.redBits);
				glfwWindowHint(GLFW_GREEN_BITS, info.greenBits);
				glfwWindowHint(GLFW_BLUE_BITS, info.blueBits);
				glfwWindowHint(GLFW_ALPHA_BITS, info.alphaBits);
				glfwWindowHint(GLFW_DEPTH_BITS, info.depthBits);
				glfwWindowHint(GLFW_STENCIL_BITS, info.stencilBits);
				foundFormat = true;
				break;
			}
		}
		LOG_GLFW_ASSERT(foundFormat, "Could not find a matching format!");

		glfwWindowHint(GLFW_SAMPLES, spec.Device.swapChainSampleCount);
		glfwWindowHint(GLFW_REFRESH_RATE, spec.Device.refreshRate);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, spec.Window.resizeWindowWithDisplayScale);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Ignored for fullscreen

		if (spec.Window.startBorderless)
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Borderless window
		if (spec.Window.allowResizing)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		// finally create the window
		m_Window = glfwCreateWindow(spec.Device.backBufferWidth, spec.Device.backBufferHeight,
			spec.Window.windowTitle.c_str(),
			spec.Window.startFullscreen ? glfwGetPrimaryMonitor() : nullptr,
			nullptr);
		LOG_GLFW_ASSERT(m_Window, "Could not create the window!");
		s_GLFWWindowCount++;

		glfwSetWindowUserPointer(m_Window, &m_Data);

		if (spec.Window.startFullscreen) {
			glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0,
				spec.Device.backBufferWidth, spec.Device.backBufferHeight, spec.Device.refreshRate);
		} else {
			int fbWidth = 0, fbHeight = 0;
			glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
			spec.Device.backBufferWidth = fbWidth;
			spec.Device.backBufferHeight = fbHeight;
		}

		if (spec.Window.windowPosX != -1 && spec.Window.windowPosY != -1)
			glfwSetWindowPos(m_Window, spec.Window.windowPosX, spec.Window.windowPosY);

		InitCallbacks();

//		CreateDevice()
//		CreateSwapChain()

		glfwShowWindow(m_Window);

		if (spec.Window.startMaximized)
			glfwMaximizeWindow(m_Window);

		// reset the back buffer size state to enforce a resize event
		spec.Device.backBufferWidth = 0;
		spec.Device.backBufferHeight = 0;
//		UpdateWindowSize();

		// TODO...
		// TODO...
		// TODO...
	}

	Window::~Window()
	{
		// drestroy window
		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		// terminate GLFW if it was the last window
		if (s_GLFWWindowCount == 0)
			glfwTerminate();
	}


	void Window::OnUpdate()
	{
		glfwPollEvents();
	}


	void Window::InitCallbacks()
	{
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
				if (data.EventCallback)
					data.EventCallback(event);
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowResizeEvent event(width, height);
				LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
				if (data.EventCallback)
					data.EventCallback(event);
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowMovedEvent event(xpos, ypos);
				LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
				if (data.EventCallback)
					data.EventCallback(event);
			});

//		glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* window)
//			{
//				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//				xxxxxxxxxxxxxxxxEvent event();
//				if (data.EventCallback)
//					data.EventCallback(event);
//			});

//		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
//			{
//				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//				xxxxxxxxxxxxxxxxEvent event(focused);
//				if (data.EventCallback)
//					data.EventCallback(event);
//			});

//		glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified)
//			{
//				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//				xxxxxxxxxxxxxxxxEvent event(iconified);
//				if (data.EventCallback)
//					data.EventCallback(event);
//			});

//		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized)
//			{
//				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//				xxxxxxxxxxxxxxxxEvent event(maximized);
//				if (data.EventCallback)
//					data.EventCallback(event);
//			});

//		glfwSetWindowContentScaleCallback(m_Window, [](GLFWwindow* window, float xscale, float yscale)
//			{
//				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//				xxxxxxxxxxxxxxxxEvent event(xscale, yscale);
//				if (data.EventCallback)
//					data.EventCallback(event);
//			});

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				FramebufferResizeEvent event(width, height);
				LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
				if (data.EventCallback)
					data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
					if (data.EventCallback)
						data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
					if (data.EventCallback)
						data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xpos, (float)ypos);
				//LOG_RENDER_TRACE("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
				if (data.EventCallback)
					data.EventCallback(event);
			});

//		glfwSetCursorEnterCallback(m_Window, [](GLFWwindow* window, int entered)
//			{
//				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//				xxxxxxxxxxxxxxxxEvent event(width, height);
//				if (data.EventCallback)
//					data.EventCallback(event);
//			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xoffset, (float)yoffset);
				LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
				if (data.EventCallback)
					data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
					if (data.EventCallback)
						data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
					if (data.EventCallback)
						data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
					if (data.EventCallback)
						data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent event(keycode);
				LOG_GLFW_DEBUG("GLFWCallback: {} {}", event.ToString(), data.EventCallback ? "" : "(unset)");
				if (data.EventCallback)
					data.EventCallback(event);
			});

//		glfwSetCharModsCallback(m_Window, [](GLFWwindow* window, unsigned int codepoint, int mods)
//			{
//				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//				xxxxxxxxxxxxxxxxEvent event(width, height);
//				if (data.EventCallback)
//					data.EventCallback(event);
//			});

//		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int path_count, const char* paths[])
//			{
//				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//				xxxxxxxxxxxxxxxxEvent event(width, height);
//				if (data.EventCallback)
//					data.EventCallback(event);
//			});
	}


} // end of namespace Helios::Engine





