#include "pch.h"
#include "Platform/Renderer/Vulkan/VKWindow.h"

#include "Helios/Engine/Renderer/Format.h"
#include "Helios/Engine/Spec/SpecWindow.h"
#include "Helios/Engine/Spec/SpecDevice.h"

#ifdef TARGET_PLATFORM_WINDOWS
#	include <ShellScalingApi.h>
#	pragma comment(lib, "shcore.lib")
#endif

namespace Helios::Engine::Renderer {


	VKWindow::VKWindow()
	{
		LOG_RENDER_DEBUG("VKWindow: Creating window.");

#		ifdef TARGET_PLATFORM_WINDOWS
			if (!Spec::Window::enablePerMonitorDPI)
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

		if (Spec::Device::swapChainFormat == Renderer::Format::UNKNOWN) {
			LOG_RENDER_WARN("Swap chain format is UNKNOWN! Using default GLFW window hints for color and depth/stencil bits.");
		} else {
			//--------------------------------------------------------------------------------------
			// TODO: Verify that the requested format is compatible with GLFW window hints
			// TODO: Verify that the requested format is supported by the physical device surface
			//--------------------------------------------------------------------------------------
			const Renderer::FormatInfo& formatInfo = Renderer::GetFormatInfo(Spec::Device::swapChainFormat);
			glfwWindowHint(GLFW_RED_BITS,   formatInfo.redBits);
			glfwWindowHint(GLFW_GREEN_BITS, formatInfo.greenBits);
			glfwWindowHint(GLFW_BLUE_BITS,  formatInfo.blueBits);
			glfwWindowHint(GLFW_ALPHA_BITS, formatInfo.alphaBits);
			glfwWindowHint(GLFW_DEPTH_BITS, formatInfo.depthBits);
			glfwWindowHint(GLFW_STENCIL_BITS, formatInfo.stencilBits);
		}

		glfwWindowHint(GLFW_SAMPLES, Spec::Device::swapChainSampleCount);
		glfwWindowHint(GLFW_REFRESH_RATE, Spec::Window::refreshRate);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, Spec::Window::resizeWindowWithDisplayScale);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Ignored for fullscreen

		if (Spec::Window::startBorderless)
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Borderless window
		if (Spec::Window::allowResizing)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		// finally create the window
		m_Window = glfwCreateWindow(Spec::Window::sizeX, Spec::Window::sizeY,
			Spec::Window::windowTitle.c_str(),
			Spec::Window::startFullscreen ? glfwGetPrimaryMonitor() : nullptr,
			nullptr);
		LOG_GLFW_ASSERT(m_Window, "Could not create the window!");
//		s_GLFWWindowCount++;

		if (Spec::Window::startFullscreen) {
			glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0,
				Spec::Window::sizeX, Spec::Window::sizeY, Spec::Window::refreshRate);
		} else {
			int fbWidth = 0, fbHeight = 0;
			glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
			Spec::Window::sizeX = fbWidth;
			Spec::Window::sizeY = fbHeight;
		}

		if (Spec::Window::posX != -1 && Spec::Window::posY != -1)
			glfwSetWindowPos(m_Window, Spec::Window::posX, Spec::Window::posY);

		InitCallbacks();
	}


	VKWindow::~VKWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
		LOG_RENDER_DEBUG("VKWindow: Destroyed window.");
	}


	void VKWindow::OnUpdate()
	{
		glfwPollEvents();
	}


	void VKWindow::Show()
	{
		glfwShowWindow(m_Window);

		if (Spec::Window::startMaximized)
			glfwMaximizeWindow(m_Window);

		// reset the back buffer size state to enforce a resize event
//		Spec::Window::sizeX = 0;
//		Spec::Window::sizeY = 0;
//		UpdateWindowSize();
	}


	void VKWindow::Hide()
	{
		glfwHideWindow(m_Window);
	}


} // namespace Helios::Engine::Renderer
