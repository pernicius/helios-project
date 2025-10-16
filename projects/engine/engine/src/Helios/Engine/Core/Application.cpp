#include "pch.h"
#include "Helios/Engine/Core/Application.h"

#include <Helios/Util/Version.h>

#include <Platform/PlatformDetection.h>
#if defined TARGET_PLATFORM_WINDOWS
#	include "Platform/System/Windows/WinUtil.h"
#elif defined TARGET_PLATFORM_LINUX
#	include "Platform/System/Linux/LinuxUtil.h"
#elif defined TARGET_PLATFORM_MACOS
#	include "Platform/System/MacOS/MacOSUtil.h"
#endif

#include <GLFW/glfw3.h>

#include <string>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <cctype>
#include <iostream>

namespace Helios::Engine {


	// ----------------------------------------------------------------------------------------------------
	
	
	int AppMain(int argc, char** argv)
	{
		try {
			auto app = CreateApplication({ argc, argv });
			app->Run();
			delete app;
			return EXIT_SUCCESS;
		}
		catch (const std::exception& e) {
			std::cerr << "Application terminated with exception: " << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	
	
	// ----------------------------------------------------------------------------------------------------


	bool Application::CommandLineArgs::Check(std::string arg)
	{
		for (auto x = 1; x < Count; x++)
		{
			size_t start = std::string(Args[x]).find_first_not_of("/-");
			std::string trimmed = std::string(Args[x]).substr(start);
			trimmed = trimmed.substr(0, trimmed.find_first_of('='));

			std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(),
				[](unsigned char c) { return std::tolower(c); });

			if (trimmed == arg)
				return true;
		}
		return false;
	}


	std::string Application::CommandLineArgs::Get(std::string arg, std::string default_value)
	{
		for (auto x = 1; x < Count; x++)
		{
			size_t start = std::string(Args[x]).find_first_not_of("/-");
			std::string trimmed = std::string(Args[x]).substr(start);

			std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(),
				[](unsigned char c) { return std::tolower(c); });

			std::string key = trimmed.substr(0, trimmed.find_first_of('='));
			if (key.compare(arg) == 0)
				return trimmed.substr(trimmed.find_first_of('=') + 1);
		}
		return default_value;
	}


	// ----------------------------------------------------------------------------------------------------


	Application::Application(const Specification& spec)
		: m_Spec(spec)
	{
		// Check singleton
		if (s_Instance) {
			LOG_CORE_EXCEPT("Application already exists!");
		}
		s_Instance = this;

		// Init working directory
		if (!m_Spec.WorkingDirectory.empty())
			std::filesystem::current_path(m_Spec.WorkingDirectory);
		if (m_Spec.hints & Hints::HINT_USE_CWD)
			m_Spec.WorkingDirectory = std::filesystem::current_path().string();
		if (m_Spec.hints & Hints::HINT_USE_EXEPATH)
		{
			m_Spec.WorkingDirectory = Util::GetExecutablePath();
			std::filesystem::current_path(m_Spec.WorkingDirectory);
		}

		// Init logging
		Log::Init(m_Spec.logfile, m_Spec.WorkingDirectory);
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
			spec.Name);
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

		// Log and "parse" CmdArgs
		if (m_Spec.CmdLineArgs.Count > 1)
		{
			for (auto x = 1; x < m_Spec.CmdLineArgs.Count; x++) {
				LOG_CORE_INFO("CmdArg[{}] = \"{}\"", x, m_Spec.CmdLineArgs[x]);

#				ifdef HE_RENDERER_VULKAN
				if (m_Spec.CmdLineArgs[x] == "--vulkan")
{}//					Config::Override("RendererAPI", "Vulkan");
#				endif

#				ifdef HE_RENDERER_OPENGL
				if (m_Spec.CmdLineArgs[x] == "--opengl")
{}//					Config::Override("RendererAPI", "OpenGL");
#				endif

#				ifdef HE_RENDERER_METAL
				if (m_Spec.CmdLineArgs[x] == "--metal")
{}//					Config::Override("RendererAPI", "Metal");
#				endif

#				ifdef HE_RENDERER_DX12
				if (m_Spec.CmdLineArgs[x] == "--dx12")
{}//					Config::Override("RendererAPI", "DX12");
#				endif

#				ifdef HE_RENDERER_DX11
				if (m_Spec.CmdLineArgs[x] == "--dx11")
{}//					Config::Override("RendererAPI", "DX11");
#				endif
			}
		}
	}


	Application::~Application()
	{
		LOG_CORE_INFO("App Shutdown.");

//		Config::Save();
//		Renderer::Shutdown();

		s_Instance = nullptr;
	}


//	void Application::PushLayer(Layer* layer)
//	{
//		m_LayerStack.PushLayer(layer);
//		layer->OnAttach();
//
//		int size_x, size_y;
//		glfwGetWindowSize((GLFWwindow*)DeviceManager::GetMainWindow()->GetNativeWindow(), &size_x, &size_y);
//		WindowResizeEvent event(size_x, size_y);
//		OnEvent(event);
//	}


//	void Application::PushOverlay(Layer* layer)
//	{
//		m_LayerStack.PushOverlay(layer);
//		layer->OnAttach();
//
//		int size_x, size_y;
//		glfwGetWindowSize((GLFWwindow*)DeviceManager::GetMainWindow()->GetNativeWindow(), &size_x, &size_y);
//		WindowResizeEvent event(size_x, size_y);
//		OnEvent(event);
//	}


	void Application::CreateAppWindow()
	{
//		auto wnd = DeviceManager::CreateMainWindow();
//		wnd->SetEventCallback(HE_BIND_EVENT_FN(OnEvent));
//		wnd->Show();

//		CreateDevice()
//		CreateSwapChain()
		// TODO...
		// TODO...
		// TODO...
	}


	void Application::Close()
	{
		m_Running = false;
	}


//	void Application::OnEvent(Event& e)
//	{
//		EventDispatcher dispatcher(e);
//		dispatcher.Dispatch<WindowCloseEvent>(HE_BIND_EVENT_FN(Application::OnWindowClose));
//		dispatcher.Dispatch<WindowResizeEvent>(HE_BIND_EVENT_FN(Application::OnWindowResize));
//		dispatcher.Dispatch<FramebufferResizeEvent>(HE_BIND_EVENT_FN(Application::OnFramebufferResize));
//
//		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
//		{
//			if (e.Handled)
//				break;
//			(*it)->OnEvent(e);
//		}
//	}


	void Application::Run()
	{
//		Timer RunLoopTimer;
		while (m_Running)
		{
//			Timestep timestep = RunLoopTimer.Elapsed();
//			RunLoopTimer.Reset();

#if 0
			{ // tempoary for debuging
				static int fps = 0;
				static int fps_cnt = 0;
				static float fps_ts = 0;
				fps += (int)(1.0f / timestep);
				fps_cnt++;
				fps_ts += timestep;
				if (fps_ts >= 1.0f)
				{
					std::ostringstream title;
					title << "FPS: " << fps / fps_cnt << " (" << 1000 * fps_ts / fps_cnt << " ms)";
					glfwSetWindowTitle((GLFWwindow*)m_Window->GetNativeWindow(), title.str().c_str());
					fps = 0;
					fps_cnt = 0;
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

			// Rendering
			if (!m_Minimized)
			{
				// Update
//				for (Layer* layer : m_LayerStack)
//					layer->OnUpdate(timestep);

				// Render
//				for (Layer* layer : m_LayerStack)
//					layer->OnRender(timestep);

				// Render
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
//			DeviceManager::GetMainWindow()->OnUpdate();
		}
	}


//	bool Application::OnWindowClose(WindowCloseEvent& e)
//	{
//		m_Running = false;
//		return true;
//	}


//	bool Application::OnWindowResize(WindowResizeEvent& e)
//	{
//		if (e.GetWidth() == 0 || e.GetHeight() == 0)
//		{
//			m_Minimized = true;
//			return false;
//		}
//
//		m_Minimized = false;
//		//		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
//
//		return false;
//	}


//	bool Application::OnFramebufferResize(FramebufferResizeEvent& e)
//	{
//		if (e.GetWidth() == 0 || e.GetHeight() == 0)
//		{
//			m_Minimized = true;
//			return false;
//		}
//
//		m_Minimized = false;
//		//		Renderer::OnFramebufferResize(e.GetWidth(), e.GetHeight());
//
//		return false;
//	}


} // namespace Helios::Engine
