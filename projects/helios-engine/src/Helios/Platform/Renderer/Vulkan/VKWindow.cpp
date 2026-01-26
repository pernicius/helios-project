//==============================================================================
// Vulkan Window Implementation
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VKWindow.h
//==============================================================================
#include "pch.h"
#include "Helios/Platform/Renderer/Vulkan/VKWindow.h"

#include "Helios/Engine/Core/Config.h"
//#include "Helios/Engine/Renderer/Format.h"
//#include "Helios/Engine/Spec/SpecWindow.h"
//#include "Helios/Engine/Spec/SpecDevice.h"

#ifdef TARGET_PLATFORM_WINDOWS
#	include <ShellScalingApi.h>
#	pragma comment(lib, "shcore.lib")
#endif

namespace Helios::Engine::Renderer::Vulkan {


//------------------------------------------------------------------------------
// Configuration value shortcuts

#define CFG_SwapChainSampleCount   ConfigManager::GetInstance().Get<int>( "HeliosEngine/Window", "Config", "SwapChainSampleCount",   1)
#define CFG_ResizeWithDisplayScale ConfigManager::GetInstance().Get<bool>("HeliosEngine/Window", "Config", "ResizeWithDisplayScale", false)
#define CFG_StartBorderless        ConfigManager::GetInstance().Get<bool>("HeliosEngine/Window", "Config", "StartBorderless",        false)
#define CFG_AllowResizing          ConfigManager::GetInstance().Get<bool>("HeliosEngine/Window", "Config", "AllowResizing",          true)
#define CFG_RefreshRate            ConfigManager::GetInstance().Get<int>( "HeliosEngine/Window", "Config", "RefreshRate",            60)
#define CFG_DisableDPIAwareness    ConfigManager::GetInstance().Get<bool>("HeliosEngine/Window", "Config", "DisableDPIAwareness",    false) /* Windows only */

//------------------------------------------------------------------------------
// Configuration value shortcuts (Vulkan specific)

#define CFG_SwapChainFormat        vk::Format::eB8G8R8A8Unorm

#define CFG_Title                  "test"

//------------------------------------------------------------------------------


	void VKWindow::ConfigureWindowHints()
	{
		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		glfwWindowHint(GLFW_SAMPLES,           CFG_SwapChainSampleCount);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR,  CFG_ResizeWithDisplayScale);
		glfwWindowHint(GLFW_DECORATED,        !CFG_StartBorderless);
		glfwWindowHint(GLFW_RESIZABLE,         CFG_AllowResizing);

		glfwWindowHint(GLFW_REFRESH_RATE,      CFG_RefreshRate);

//		if (Spec::Device::swapChainFormat == Format::UNKNOWN) {
//			LOG_RENDER_WARN("Swap chain format is UNKNOWN! Using default GLFW window hints for color and depth/stencil bits.");
//		} else {
//			//--------------------------------------------------------------------------------------
//			// TODO: Verify that the requested format is compatible with GLFW window hints
//			// TODO: Verify that the requested format is supported by the physical device surface
//			//--------------------------------------------------------------------------------------
//			const Renderer::FormatInfo& formatInfo = Renderer::GetFormatInfo(CFG_SwapChainFormat);
//			glfwWindowHint(GLFW_RED_BITS,   formatInfo.redBits);
//			glfwWindowHint(GLFW_GREEN_BITS, formatInfo.greenBits);
//			glfwWindowHint(GLFW_BLUE_BITS,  formatInfo.blueBits);
//			glfwWindowHint(GLFW_ALPHA_BITS, formatInfo.alphaBits);
//			glfwWindowHint(GLFW_DEPTH_BITS, formatInfo.depthBits);
//			glfwWindowHint(GLFW_STENCIL_BITS, formatInfo.stencilBits);
//		}
	}


	VKWindow::VKWindow(const std::string name)
		: Window(name)
	{
		LOG_RENDER_DEBUG("VKWindow: Creating window.");

		if (s_WindowCount == 0) {
			// Initialize GLFW library once
			LOG_RENDER_DEBUG("VKWindow: Initializing GLFW library.");

	#		ifdef TARGET_PLATFORM_WINDOWS
				if (CFG_DisableDPIAwareness) {
					// glfwInit enables the maximum supported level of DPI awareness unconditionally.
					// If the app doesn't need it, we have to call this function before glfwInit to override that behavior.
					SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
				}
	#		endif

			int success = glfwInit();
			LOG_GLFW_ASSERT(success, "Could not initialize GLFW!");
		}

		// Configure window hints
		ConfigureWindowHints();

		// Create window (hidden initially)
		m_Window = glfwCreateWindow(m_windowedState.sizeX, m_windowedState.sizeY, CFG_Title, nullptr, nullptr);
		LOG_GLFW_ASSERT(m_Window, "Could not create the window!");

		// Apply position/size
		if (m_windowedState.posX != -1 and m_windowedState.posY != -1)
			glfwSetWindowPos(m_Window, m_windowedState.posX, m_windowedState.posY);
		glfwSetWindowSize(m_Window, m_windowedState.sizeX, m_windowedState.sizeY);

		// Show window
		Show();
		glfwFocusWindow(m_Window);

		// Apply maximize/minimize
		if (m_currentState.isMaximized)
			glfwMaximizeWindow(m_Window);
		else if (m_currentState.isMinimized)
			glfwIconifyWindow(m_Window);

		// Apply fullscreen
		if (m_currentState.isFullscreen)
			ToggleFullscreen();
		m_monitorState.monitor = GetCurrentMonitor();

		// Initialize callbacks
		InitCallbacks();

		// Force some events to update the application accordingly
		{
			// Force window move event
			int px = 0, py = 0;
			glfwGetWindowPos(m_Window, &px, &py);
			RaiseEvent(CreateScope<WindowMovedEvent>(px, py));
			// Force window resize event
			int sx = 0, sy = 0;
			glfwGetWindowSize(m_Window, &sx, &sy);
			RaiseEvent(CreateScope<WindowResizeEvent>(sx, sy));
			// Force framebuffer resize event
			int fbx, fby;
			glfwGetFramebufferSize(m_Window, &fbx, &fby);
			RaiseEvent(CreateScope<FramebufferResizeEvent>(fbx, fby));
		}

		// some logging
		{
			LOG_RENDER_TRACE("Video modes of selected monitor ({}):", GetCurrentMonitorName());
			int count;
			const GLFWvidmode* modes = glfwGetVideoModes(m_monitorState.monitor, &count);
			for (int i = 0; i < count; ++i) {
				LOG_RENDER_TRACE(" - ({}) {} x {} at {}Hz",
					(i+1),
					modes[i].width, modes[i].height,
					modes[i].refreshRate);
			}
		}
	}


	VKWindow::~VKWindow()
	{
		glfwDestroyWindow(m_Window);
		if (s_WindowCount == 1)
			glfwTerminate();
		LOG_RENDER_DEBUG("VKWindow: Destroyed window.");
	}


} // namespace Helios::Engine::Renderer::Vulkan
