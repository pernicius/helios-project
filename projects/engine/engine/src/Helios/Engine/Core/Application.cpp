#include "pch.h"
#include "Helios/Engine/Core/Application.h"

#include "Helios/Engine/Events/EventManager.h"
#include <Helios/Engine/Renderer/RendererAPI.h>

#include <Helios/Util/Version.h>
#include <Helios/Util/ScopeRef.h>

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
#include <string_view>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <cctype>
#include <iostream>

namespace Helios::Engine {


	namespace { // internal helpers
		inline bool ci_equal(std::string_view a, std::string_view b) noexcept
		{
			if (a.size() != b.size()) return false;
			for (size_t i = 0; i < a.size(); ++i)
			{
				if (std::tolower(static_cast<unsigned char>(a[i])) !=
					std::tolower(static_cast<unsigned char>(b[i])))
					return false;
			}
			return true;
		}

		inline void split_arg(std::string_view raw, std::string_view& out_key, std::string_view& out_val) noexcept
		{
			out_key = {};
			out_val = {};

			size_t start = raw.find_first_not_of("/-");
			if (start == std::string_view::npos)
				return;

			raw.remove_prefix(start);
			size_t eq_pos = raw.find('=');
			out_key = raw.substr(0, eq_pos);
			if (eq_pos != std::string_view::npos)
				out_val = raw.substr(eq_pos + 1);
		}
	} // internal helpers


	// ----------------------------------------------------------------------------------------------------
	
	static bool g_AppNeedRestart = true;

	int AppMain(int argc, char** argv)
	{
		int rval = EXIT_FAILURE;

		while (g_AppNeedRestart)
		{
			g_AppNeedRestart = false;

			try {
				auto app = Scope<Application>(CreateApplication({ argc, argv }));
				app->Run();
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


	bool Application::CommandLineArgs::Check(std::string_view arg) const
	{
		if (arg.empty())
			return false;

		for (int x = 1; x < Count; ++x)
		{
			std::string_view raw_arg(Args[x]);
			std::string_view key, value;
			split_arg(raw_arg, key, value);

			if (key.empty())
				continue;

			if (ci_equal(key, arg))
				return true;
		}
		return false;
	}

	std::string Application::CommandLineArgs::Get(std::string_view arg, std::string_view default_value) const
	{
		if (arg.empty())
			return std::string(default_value);

		for (int x = 1; x < Count; ++x)
		{
			std::string_view raw_arg(Args[x]);
			std::string_view key, value;
			split_arg(raw_arg, key, value);

			if (key.empty())
				continue;

			if (ci_equal(key, arg))
			{
				if (!value.empty())
					return std::string(value);
				return std::string(default_value);
			}
		}
		return std::string(default_value);
	}


	// ----------------------------------------------------------------------------------------------------


	Application::Application(const Specification& spec)
		: m_Spec(spec)
		, m_WindowCloseCallback([this](const Events::WindowCloseEvent& e) { OnWindowClose(e); })
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
				LOG_CORE_INFO("CmdArg[] = \"{}\"", m_Spec.CmdLineArgs[x]);
			}
		}

		// Init renderer
		RendererAPI::CheckAPISupport();
#		ifdef HE_RENDERER_VULKAN
			if (m_Spec.CmdLineArgs.Check("vulkan")) {
//				Config::Override("RendererAPI", "Vulkan");
				LOG_CORE_INFO("Overriding renderer API to Vulkan by command-line-switch");
				RendererAPI::SetAPI(RendererAPI::API::Vulkan);
			}
#		endif
#		ifdef HE_RENDERER_OPENGL
			if (m_Spec.CmdLineArgs.Check("opengl")) {
				LOG_CORE_INFO("Overriding renderer API to OpenGL by command-line-switch");
				RendererAPI::SetAPI(RendererAPI::API::OpenGL);
//				Config::Override("RendererAPI", "OpenGL");
			}
#		endif
#		ifdef HE_RENDERER_METAL
			if (m_Spec.CmdLineArgs.Check("metal")) {
				LOG_CORE_INFO("Overriding renderer API to Metal by command-line-switch");
				RendererAPI::SetAPI(RendererAPI::API::Metal);
//				Config::Override("RendererAPI", "Metal");
			}
#		endif
#		ifdef HE_RENDERER_DIRECTX
			if (m_Spec.CmdLineArgs.Check("directx")) {
				LOG_CORE_INFO("Overriding renderer API to DirectX by command-line-switch");
				RendererAPI::SetAPI(RendererAPI::API::DirectX);
//				Config::Override("RendererAPI", "DirectX");
			}
#		endif

		// Events
		Events::Subscribe<Events::WindowCloseEvent>(m_WindowCloseCallback);
	}


	Application::~Application()
	{
		LOG_CORE_INFO("App Shutdown.");

		m_Window.reset();

//		Config::Save();
//		Renderer::Shutdown();
		Events::Shutdown();
		Log::Shutdown();

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
		m_Window = Window::Create();
		m_Window->Show();

//		CreateDevice()
//		CreateSwapChain()
		// TODO...
		// TODO...
		// TODO...
	}


	bool Application::NeedRestart(bool setRestart)
	{
		if (setRestart)
			g_AppNeedRestart = true;
		return g_AppNeedRestart;
	}


//	void Application::Close()
//	{
//		m_Running = false;
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
			m_Window->OnUpdate();
			Events::Dispatch();
		}
	}


	void Application::OnWindowClose(const Events::WindowCloseEvent& e)
	{
		LOG_CORE_INFO("Window close event received.");
		m_Running = false;
	}


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
