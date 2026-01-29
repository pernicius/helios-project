//==============================================================================
// Application Core Implementation
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file Application.h
//==============================================================================
#include "pch.h"
#include "Helios/Engine/Core/Application.h"

#include "Helios/Engine/Core/Timer.h"
#include "Helios/Engine/Core/Timestep.h"
#include "Helios/Engine/Core/Config.h"
#include "Helios/Engine/VFS/VFS.h"

#include "Helios/Engine/Renderer/RendererAPI.h"
#include "Helios/Engine/Renderer/Renderer.h"

#include <Helios/Platform/PlatformDetection.h>
#if defined TARGET_PLATFORM_WINDOWS
#	include "Helios/Platform/System/Windows/WinUtil.h"
#elif defined TARGET_PLATFORM_LINUX
#	include "Helios/Platform/System/Linux/LinuxUtil.h"
#elif defined TARGET_PLATFORM_MACOS
#	include "Helios/Platform/System/MacOS/MacOSUtil.h"
#endif

// Configuration
#define IMMEDIATE_EVENT_PROCESSING 1

namespace Helios::Engine {


	static bool g_AppNeedRestart = true;

	int AppMain(int argc, char** argv)
	{
		int rval = EXIT_FAILURE;

		while (g_AppNeedRestart)
		{
			g_AppNeedRestart = false;

			try {
				auto app = Scope<Application>(CreateApplication(argc, argv));
				app->OnInit();
				app->Run();
				app->OnShutdown();
				app.reset();

				rval = EXIT_SUCCESS;
			}
			catch (const std::exception& e) {
				std::cerr << "Application terminated with exception: " << e.what() << std::endl;
			}
			catch (...) {
				std::cerr << "Application terminated with unknown exception." << std::endl;
			}
		}

		return rval;
	}


	// ----------------------------------------------------------------------------------------------------


	Application::Application(const AppSpec& spec)
		: m_Spec(spec)
	{
		// Check singleton
		if (s_Instance)
			LOG_CORE_EXCEPT("Application already exists!");
		s_Instance = this;

		// Init working directory
		if (m_Spec.Hints & AppSpec::HintFlags::USE_CWD) {
			m_Spec.WorkingDirectory = std::filesystem::current_path().string();
		}
		if (m_Spec.Hints & AppSpec::HintFlags::USE_EXEPATH) {
			m_Spec.WorkingDirectory = Util::GetExecutablePath();
		}
		std::filesystem::current_path(m_Spec.WorkingDirectory);

		// Init logging
		Log::Init(m_Spec.LogFile, m_Spec.WorkingDirectory);
		LOG_CORE_INFO("Logging started.");

		// Log versions
		LOG_CORE_INFO("Engine-Version: {}.{}.{} ({})",
			HE_VERSION_MAJOR(HE_VERSION),
			HE_VERSION_MINOR(HE_VERSION),
			HE_VERSION_PATCH(HE_VERSION),
			HE_VERSION_TYPE_STRING(HE_VERSION));
		LOG_CORE_INFO("Application-Version: {}.{}.{} ({}) - {}",
			HE_VERSION_MAJOR(m_Spec.Version),
			HE_VERSION_MINOR(m_Spec.Version),
			HE_VERSION_PATCH(m_Spec.Version),
			HE_VERSION_TYPE_STRING(m_Spec.Version),
			m_Spec.Name);
		LOG_CORE_DEBUG("Lib \"GLFW\": {}.{}.{}",
			GLFW_VERSION_MAJOR,
			GLFW_VERSION_MINOR,
			GLFW_VERSION_REVISION);
		LOG_CORE_DEBUG("Lib \"spdlog\": {}.{}.{}",
			SPDLOG_VER_MAJOR,
			SPDLOG_VER_MINOR,
			SPDLOG_VER_PATCH);
//		LOG_CORE_DEBUG("Lib \"EnTT\": {}.{}.{}",
//			ENTT_VERSION_MAJOR,
//			ENTT_VERSION_MINOR,
//			ENTT_VERSION_PATCH);
//		LOG_CORE_DEBUG("Lib \"GLM\": {}.{}.{}",
//			GLM_VERSION_MAJOR,
//			GLM_VERSION_MINOR,
//			GLM_VERSION_PATCH);
		LOG_CORE_DEBUG("Working path: {}", m_Spec.WorkingDirectory);

		// Read config
//		Config::Read(m_Spec.configfile, m_Spec.WorkingDirectory);

		// Init VFS (aliases for ConfigManager)
		VirtFS.Mount("config", m_Spec.WorkingDirectory + "/config", 0, "HeliosEngine", false);
		VirtFS.CreateAlias("@config_default:",  "config/default");
#if defined TARGET_PLATFORM_WINDOWS
		VirtFS.CreateAlias("@config_platform:", "config/windows");
#elif defined TARGET_PLATFORM_LINUX
		VirtFS.CreateAlias("@config_platform:", "config/linux");
#elif defined TARGET_PLATFORM_MACOS
		VirtFS.CreateAlias("@config_platform:", "config/macos");
#endif
		VirtFS.CreateAlias("@config_project:",  "config/" + m_Spec.Name);
		VirtFS.CreateAlias("@config_user:",     "config/" + m_Spec.ConfigUser);

		// Init ConfigManager
//		ConfigManager::GetInstance().LoadDomain("HeliosEngine/Window", "window_app.ini");

		// Log and "parse" CmdArgs
		if (m_Spec.CmdLineArgs.Count > 1)
		{
			for (auto x = 1; x < m_Spec.CmdLineArgs.Count; x++) {
				LOG_CORE_INFO("CmdArg[] = \"{}\"", m_Spec.CmdLineArgs[x]);
			}
		}

		// Init renderer
		Renderer::RendererAPI::CheckAPISupport();
#		ifdef HE_RENDERER_VULKAN
			if (m_Spec.CmdLineArgs.Check("vulkan")) {
				LOG_CORE_INFO("Overriding renderer API to Vulkan by command-line-switch");
				Renderer::RendererAPI::SetAPI(Renderer::RendererAPI::API::Vulkan);
//				Config::Override("RendererAPI", "Vulkan");
			}
#		endif
#		ifdef HE_RENDERER_DIRECTX
			if (m_Spec.CmdLineArgs.Check("directx")) {
				LOG_CORE_INFO("Overriding renderer API to DirectX by command-line-switch");
				Renderer::RendererAPI::SetAPI(Renderer::RendererAPI::API::DirectX);
//				Config::Override("RendererAPI", "DirectX");
			}
#		endif
#		ifdef HE_RENDERER_METAL
			if (m_Spec.CmdLineArgs.Check("metal")) {
				LOG_CORE_INFO("Overriding renderer API to Metal by command-line-switch");
				Renderer::RendererAPI::SetAPI(Renderer::RendererAPI::API::Metal);
//				Config::Override("RendererAPI", "Metal");
			}
#		endif

	}


	Application::~Application()
	{
		LOG_CORE_INFO("Application: Shutdown.");

//		Config::Save();

		// Shutdown Renderer
		if (m_Renderer) {
			m_Renderer->Shutdown();
			m_Renderer.reset();
		}
		//		m_DeviceManager.reset();
		m_Window.reset();
		
		// Shutdown application components
		Log::Shutdown();

		s_Instance = nullptr;
	}


	void Application::InitRenderer()
	{
		// Main window
		m_Window = Renderer::Window::Create(m_Spec.Name);
		m_Window->Show();

		m_Renderer = Renderer::Renderer::Create();
		if (m_Renderer) {
			m_Renderer->Init(m_Spec, *m_Window);
		}
		else {
			LOG_CORE_FATAL("Failed to create Renderer!");
			m_Running = false;
		}

//		m_DeviceManager = Renderer::DeviceManager::Create();
	}


	bool Application::NeedRestart(bool setRestart)
	{
		if (setRestart)
			g_AppNeedRestart = true;
		return g_AppNeedRestart;
	}


	void Application::Run()
	{
		TimerSec RunLoopTimer;
		while (m_Running)
		{
			Timestep timestep = RunLoopTimer.Elapsed();
			RunLoopTimer.Reset();

#if 0
			{ // tempoary for debuging
				static int fps = 0;
				static float fps_ts = 0;
				fps++;
				fps_ts += timestep;
				if (fps_ts >= 1.0f)
				{
					std::ostringstream title;
					title << "FPS: " << fps << " (" << (fps_ts / fps) << "s)";
					glfwSetWindowTitle((GLFWwindow*)m_Window->GetNativeWindow(), title.str().c_str());
					fps = 0;
					fps_ts = 0;
				}
			} // tempoary for debuging

//			{
//				static double lastTime, currentTime;
//				static int numFrames;
//				static float frameTime;
//
//				currentTime = glfwGetTime();
//				double delta = currentTime - lastTime;
//
//				if (delta >= 1)
//				{
//					int framerate{ std::max(1, int(numFrames / delta)) };
//					std::stringstream title;
//					title << "Running at " << framerate << " fps.";
//					glfwSetWindowTitle((GLFWwindow*)m_Window->GetNativeWindow(), title.str().c_str());
//					lastTime = currentTime;
//					numFrames = -1;
//					frameTime = float(1000.0 / framerate);
//				}
//				++numFrames;
//			}
#endif

			// Updating
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep);

			// Rendering (only if not minimized!)
			if (!m_Window->glfwIsMinimized())
			{
				for (Layer* layer : m_LayerStack)
					layer->OnRender();

//				Renderer::Get()->Render();

//				// ImGui rendering
//				{
//					m_ImGuiLayer->Begin();
//					//static bool show = true;
//					//ImGui::ShowDemoWindow(&show);
//					for (Layer* layer : m_LayerStack)
//						layer->OnImGuiRender();
//					m_ImGuiLayer->End();
//				}
			}

			// Poll events and so on
			glfwPollEvents();
			ProcessEvents();
		}
	}


	void Application::SubmitEvent(Scope<Event> event)
	{
		if (!event) return;

#		if (IMMEDIATE_EVENT_PROCESSING == 1)
//			LOG_CORE_TRACE("SubmitEvent (immediate): {}", event->ToString());
			OnEvent(*event);
#		else
//			LOG_CORE_TRACE("SubmitEvent: {}", event->ToString());
			std::lock_guard<std::mutex> lock(m_EventQueueMutex);
			m_EventQueue.push_back(std::move(event));
#		endif
	}


	void Application::ProcessEvents()
	{
#		if (IMMEDIATE_EVENT_PROCESSING == 0)
			std::vector<std::unique_ptr<Event>> events;
			{
				std::lock_guard<std::mutex> lock(m_EventQueueMutex);
				events.swap(m_EventQueue);
			}

			// Coalesce rapid WindowResizeEvent instances.
			// Keep the last resize event and only dispatch it when a non-resize event
			// arrives or when the queue is drained.
			Scope<WindowResizeEvent> pendingWindowResize;
			Scope<FramebufferResizeEvent> pendingFramebufferResize;

			for (auto& e : events)
			{
				if (!e) continue;

				// If this is a WindowResizeEvent, capture it and drop any previous one.
				if (e->GetEventType() == EventType::WindowResize) {
					// Transfer ownership of this specific window resize event into pendingWindowResize.
					pendingWindowResize.reset(static_cast<WindowResizeEvent*>(e.release()));
					// Do not dispatch now - wait for a stable moment or next non-resize event.
					continue;
				}

				// If this is a FramebufferResizeEvent, capture it and drop any previous one.
				if (e->GetEventType() == EventType::FramebufferResize) {
					// Transfer ownership of this specific framebuffer resize event into pendingFramebufferResize.
					pendingFramebufferResize.reset(static_cast<FramebufferResizeEvent*>(e.release()));
					// Do not dispatch now - wait for a stable moment or next non-resize event.
					continue;
				}

				// Before handling any non-resize event, flush the pending/coalesced resize (if any).
				if (pendingWindowResize) {
					if (!pendingWindowResize->Handled)
						OnEvent(*pendingWindowResize);
					pendingWindowResize.reset();
				}
				if (pendingFramebufferResize) {
					if (!pendingFramebufferResize->Handled)
						OnEvent(*pendingFramebufferResize);
					pendingFramebufferResize.reset();
				}

				// Dispatch the current non-resize event as usual.
				if (!e->Handled)
					OnEvent(*e);
			}

			// If the queue ended with resize events, dispatch the last one now.
			if (pendingWindowResize && !pendingWindowResize->Handled)
				OnEvent(*pendingWindowResize);
			if (pendingFramebufferResize && !pendingFramebufferResize->Handled)
				OnEvent(*pendingFramebufferResize);
#		endif
	}


	void Application::OnEvent(Event& e)
	{
		// Log filtering
		if (e.GetCategoryFlags() & EventCategory::Window) {
			if (e.GetEventType() != EventType::WindowMoved)
				LOG_CORE_TRACE("Application: OnEvent: {}", e.ToString());
		}

		// Dispatch event to application
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(HE_BIND_EVENT_FN(Application::OnWindowClose));

		// Forward event to renderer
		if (m_Renderer)
			m_Renderer->OnEvent(e);

		// Dispatch events to layers in reverse order
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) 
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}


	bool Application::OnWindowClose(const WindowCloseEvent& e)
	{
		LOG_CORE_DEBUG("Application: Window close event received.");
		m_Running = false;
		return false;
	}


} // namespace Helios::Engine
