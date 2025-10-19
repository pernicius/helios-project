#include "pch.h"
#include "Platform/Renderer/Vulkan/VKWindow.h"

#include "Helios/Engine/Core/Log.h"
#include "Helios/Engine/Events/EventManager.h"
#include "Helios/Engine/Events/Events_Window.h"

#ifdef TARGET_PLATFORM_WINDOWS
#	include <ShellScalingApi.h>
#	pragma comment(lib, "shcore.lib")
#endif

namespace Helios::Engine {


	VKWindow::VKWindow()
	{
#		ifdef TARGET_PLATFORM_WINDOWS
//			if (!m_Spec.Window.enablePerMonitorDPI)
//			{
//				// glfwInit enables the maximum supported level of DPI awareness unconditionally.
//				// If the app doesn't need it, we have to call this function before glfwInit to override that behavior.
//				SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
//			}
#		endif

		int success = glfwInit();
		LOG_GLFW_ASSERT(success, "Could not initialize GLFW!");

		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

//		const struct
//		{
//			nvrhi::Format format;
//			uint32_t redBits;
//			uint32_t greenBits;
//			uint32_t blueBits;
//			uint32_t alphaBits;
//			uint32_t depthBits;
//			uint32_t stencilBits;
//		} formatInfo[] = {
//			{ nvrhi::Format::UNKNOWN,            0,  0,  0,  0,  0,  0, },
//			{ nvrhi::Format::R8_UINT,            8,  0,  0,  0,  0,  0, },
//			{ nvrhi::Format::RG8_UINT,           8,  8,  0,  0,  0,  0, },
//			{ nvrhi::Format::RG8_UNORM,          8,  8,  0,  0,  0,  0, },
//			{ nvrhi::Format::R16_UINT,          16,  0,  0,  0,  0,  0, },
//			{ nvrhi::Format::R16_UNORM,         16,  0,  0,  0,  0,  0, },
//			{ nvrhi::Format::R16_FLOAT,         16,  0,  0,  0,  0,  0, },
//			{ nvrhi::Format::RGBA8_UNORM,        8,  8,  8,  8,  0,  0, },
//			{ nvrhi::Format::RGBA8_SNORM,        8,  8,  8,  8,  0,  0, },
//			{ nvrhi::Format::BGRA8_UNORM,        8,  8,  8,  8,  0,  0, },
//			{ nvrhi::Format::SRGBA8_UNORM,       8,  8,  8,  8,  0,  0, },
//			{ nvrhi::Format::SBGRA8_UNORM,       8,  8,  8,  8,  0,  0, },
//			{ nvrhi::Format::R10G10B10A2_UNORM, 10, 10, 10,  2,  0,  0, },
//			{ nvrhi::Format::R11G11B10_FLOAT,   11, 11, 10,  0,  0,  0, },
//			{ nvrhi::Format::RG16_UINT,         16, 16,  0,  0,  0,  0, },
//			{ nvrhi::Format::RG16_FLOAT,        16, 16,  0,  0,  0,  0, },
//			{ nvrhi::Format::R32_UINT,          32,  0,  0,  0,  0,  0, },
//			{ nvrhi::Format::R32_FLOAT,         32,  0,  0,  0,  0,  0, },
//			{ nvrhi::Format::RGBA16_FLOAT,      16, 16, 16, 16,  0,  0, },
//			{ nvrhi::Format::RGBA16_UNORM,      16, 16, 16, 16,  0,  0, },
//			{ nvrhi::Format::RGBA16_SNORM,      16, 16, 16, 16,  0,  0, },
//			{ nvrhi::Format::RG32_UINT,         32, 32,  0,  0,  0,  0, },
//			{ nvrhi::Format::RG32_FLOAT,        32, 32,  0,  0,  0,  0, },
//			{ nvrhi::Format::RGB32_UINT,        32, 32, 32,  0,  0,  0, },
//			{ nvrhi::Format::RGB32_FLOAT,       32, 32, 32,  0,  0,  0, },
//			{ nvrhi::Format::RGBA32_UINT,       32, 32, 32, 32,  0,  0, },
//			{ nvrhi::Format::RGBA32_FLOAT,      32, 32, 32, 32,  0,  0, },
//		};
//		bool foundFormat = false;
//		for (const auto& info : formatInfo)
//		{
//			if (info.format == m_Spec.Device.swapChainFormat)
//			{
//				glfwWindowHint(GLFW_RED_BITS, info.redBits);
//				glfwWindowHint(GLFW_GREEN_BITS, info.greenBits);
//				glfwWindowHint(GLFW_BLUE_BITS, info.blueBits);
//				glfwWindowHint(GLFW_ALPHA_BITS, info.alphaBits);
//				glfwWindowHint(GLFW_DEPTH_BITS, info.depthBits);
//				glfwWindowHint(GLFW_STENCIL_BITS, info.stencilBits);
//				foundFormat = true;
//				break;
//			}
//		}
//		LOG_GLFW_ASSERT(foundFormat, "Could not find a matching format!");

//		glfwWindowHint(GLFW_SAMPLES, m_Spec.Device.swapChainSampleCount);
//		glfwWindowHint(GLFW_REFRESH_RATE, m_Spec.Device.refreshRate);
//		glfwWindowHint(GLFW_SCALE_TO_MONITOR, m_Spec.Window.resizeWindowWithDisplayScale);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Ignored for fullscreen

//		if (m_Spec.Window.startBorderless)
//			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Borderless window
//		if (m_Spec.Window.allowResizing)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		// finally create the window
		m_Window = glfwCreateWindow(800, 600, //m_Spec.Device.backBufferWidth, m_Spec.Device.backBufferHeight,
			"test", //m_Spec.Window.windowTitle.c_str(),
			nullptr, //m_Spec.Window.startFullscreen ? glfwGetPrimaryMonitor() : nullptr,
			nullptr);
		LOG_GLFW_ASSERT(m_Window, "Could not create the window!");
//		s_GLFWWindowCount++;

//		glfwSetWindowUserPointer(m_Window, &m_Data);

//		if (m_Spec.Window.startFullscreen) {
//			glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0,
//				m_Spec.Device.backBufferWidth, m_Spec.Device.backBufferHeight, m_Spec.Device.refreshRate);
//		}
//		else {
//			int fbWidth = 0, fbHeight = 0;
//			glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
//			m_Spec.Device.backBufferWidth = fbWidth;
//			m_Spec.Device.backBufferHeight = fbHeight;
//		}

//		if (m_Spec.Window.posX != -1 && m_Spec.Window.posY != -1)
//			glfwSetWindowPos(m_Window, m_Spec.Window.posX, m_Spec.Window.posY);

		// Callbacks
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			Scope<Events::WindowCloseEvent> closeEvent = CreateScope<Events::WindowCloseEvent>();
			Events::QueueEvent(std::move(closeEvent));
			});
}


	VKWindow::~VKWindow()
	{
		LOG_INFO("Destroying VKWindow.");
	}


	void VKWindow::OnUpdate()
	{
		glfwPollEvents();
	}


	void VKWindow::Show()
	{
		glfwShowWindow(m_Window);

//		if (m_Spec.Window.startMaximized)
//			glfwMaximizeWindow(m_Window);

		// reset the back buffer size state to enforce a resize event
//		m_Spec.Device.backBufferWidth = 0;
//		m_Spec.Device.backBufferHeight = 0;
//		UpdateWindowSize();
	}


	void VKWindow::Hide()
	{
		glfwHideWindow(m_Window);
	}


}// namespace Helios::Engine
