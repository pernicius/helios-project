#include "pch.h"
#include "Platform/Renderer/Vulkan/VKWindow.h"

#include "Helios/Engine/Renderer/Format.h"
#include "Helios/Engine/Spec/SpecWindow.h"
#include "Helios/Engine/Spec/SpecDevice.h"

#ifdef TARGET_PLATFORM_WINDOWS
#	include <ShellScalingApi.h>
#	pragma comment(lib, "shcore.lib")
#endif

namespace Helios::Engine::Renderer::Vulkan {


//------------------------------------------------------------------------------
// Configuration value shortcuts

// Update-Rule: when changed (requires recreate window)
#define CFG_StartBorderless        m_Config.get<bool>("Window", "StartBorderless",              false)
// Update-Rule: when changed (requires recreate window)
#define CFG_StartFullscreen        m_Config.get<bool>("Window", "StartFullscreen",              false)
// Update-Rule: framebuffer resize events
#define CFG_SizeX                  m_Config.get<int>( "Window", "SizeX",                        800)
#define CFG_SizeY                  m_Config.get<int>( "Window", "SizeY",                        600)
// Update-Rule: window move events / -1 if in fullscreen
#define CFG_PosX                   m_Config.get<int>( "Window", "PosX",                         -1)
#define CFG_PosY                   m_Config.get<int>( "Window", "PosY",                         -1)
// Update-Rule: never
#define CFG_AllowResizing          m_Config.get<bool>("Window", "AllowResizing",                true)

#define CFG_DisableDPIAwareness    m_Config.get<bool>("Window", "DisableDPIAwareness",          false) /* Windows only */
#define CFG_SwapChainSampleCount   m_Config.get<int>( "Window", "SwapChainSampleCount",         1)
#define CFG_RefreshRate            m_Config.get<int>( "Window", "RefreshRate",                  60)
#define CFG_ResizeWithDisplayScale m_Config.get<bool>("Window", "ResizeWindowWithDisplayScale", false)

//------------------------------------------------------------------------------
// Configuration value shortcuts (Vulkan specific)

#define CFG_SwapChainFormat        Format::SBGRA8_UNORM

// TODO: convert to config value
#define CFG_Title                  Spec::Window::windowTitle.c_str()

//------------------------------------------------------------------------------


	VKWindow::VKWindow()
	{
		LOG_RENDER_DEBUG("VKWindow: Creating window.");

#		ifdef TARGET_PLATFORM_WINDOWS
			if (CFG_DisableDPIAwareness)
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

/*TODO*/		if (Spec::Device::swapChainFormat == Format::UNKNOWN) {
			LOG_RENDER_WARN("Swap chain format is UNKNOWN! Using default GLFW window hints for color and depth/stencil bits.");
		} else {
			//--------------------------------------------------------------------------------------
			// TODO: Verify that the requested format is compatible with GLFW window hints
			// TODO: Verify that the requested format is supported by the physical device surface
			//--------------------------------------------------------------------------------------
			const Renderer::FormatInfo& formatInfo = Renderer::GetFormatInfo(CFG_SwapChainFormat);
			glfwWindowHint(GLFW_RED_BITS,   formatInfo.redBits);
			glfwWindowHint(GLFW_GREEN_BITS, formatInfo.greenBits);
			glfwWindowHint(GLFW_BLUE_BITS,  formatInfo.blueBits);
			glfwWindowHint(GLFW_ALPHA_BITS, formatInfo.alphaBits);
			glfwWindowHint(GLFW_DEPTH_BITS, formatInfo.depthBits);
			glfwWindowHint(GLFW_STENCIL_BITS, formatInfo.stencilBits);
		}

		glfwWindowHint(GLFW_SAMPLES, CFG_SwapChainSampleCount);
		glfwWindowHint(GLFW_REFRESH_RATE, CFG_RefreshRate);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, CFG_ResizeWithDisplayScale);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Ignored for fullscreen

		if (CFG_StartBorderless)
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		if (CFG_AllowResizing)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		// finally create the window
		m_Window = glfwCreateWindow(CFG_SizeX, CFG_SizeY,
			CFG_Title,
			CFG_StartFullscreen ? glfwGetPrimaryMonitor() : nullptr,
			nullptr);
		LOG_GLFW_ASSERT(m_Window, "Could not create the window!");
//		s_GLFWWindowCount++;

		if (CFG_StartFullscreen) {
			glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0,
				CFG_SizeX, CFG_SizeY, CFG_RefreshRate);
		} else {
			int fbWidth = 0, fbHeight = 0;
			glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
			m_Config.set<int>("Window", "SizeX", fbWidth);
			m_Config.set<int>("Window", "SizeY", fbHeight);
		}

		if (CFG_PosX != -1 || CFG_PosY != -1)
			glfwSetWindowPos(m_Window, CFG_PosX, CFG_PosY);

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


} // namespace Helios::Engine::Renderer::Vulkan
