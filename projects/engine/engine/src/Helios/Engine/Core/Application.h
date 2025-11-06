#pragma once

#include "Helios/Engine/Core/Log.h"
//#include "Helios/Engine/Core/Layer.h"
#include "Helios/Engine/Events/EventHandler.h"
#include "Helios/Engine/Events/Types/Window.h"
//#include "Helios/Engine/Events/ApplicationEvent.h"
#include "Helios/Engine/Renderer/Window.h"

#include <string>
#include <string_view>
#include <cstdint>

namespace Helios::Engine {


	int AppMain(int argc, char** argv);


	class Application
	{
	public:
		struct CommandLineArgs
		{
			int Count = 0;
			char** Args = nullptr;

			const char* operator[](int index) const
			{
				LOG_CORE_ASSERT(index < Count, "");
				return Args[index];
			}

			bool Check(std::string_view arg) const;
			std::string Get(std::string_view arg, std::string_view default_value = {}) const;
		};

		enum Hints
		{
			HINT_USE_CWD = (1 << 0), // use the current work dir as base path
			HINT_USE_EXEPATH = (1 << 1), // use path of executeable as base path
		};

		struct Specification
		{
			// Name/Title of Application
			std::string Name;
			// Version of Application
			uint32_t Version;
			// Base path for FileIO
			std::string WorkingDirectory;
			// Command Line Arguments
			CommandLineArgs CmdLineArgs;
			// Basic configuration hints
			unsigned int hints = 0;
			// Filename of the logfile
			std::string logfile = "log.log";
			// Filename of the configfile
			std::string configfile = "config.cfg";
		};

	public:
		Application() = delete;
		Application(const Specification& specification);
		~Application();

		static Application& Get() { return *s_Instance; }
		const Specification& GetSpecification() const { return m_Spec; }
		void CreateAppWindow();
//		void Close();

		bool NeedRestart(bool setRestart = false);

//		void OnEvent(Event& e);

//		void PushLayer(Layer* layer);
//		void PushOverlay(Layer* layer);

	private:
		void Run();
//		bool OnWindowClose(WindowCloseEvent& e);
//		bool OnWindowResize(WindowResizeEvent& e);
//		bool OnFramebufferResize(FramebufferResizeEvent& e);

	private:
		Scope<Window> m_Window;
		Specification m_Spec;
		bool m_Running = true;
		bool m_Minimized = false;
//		LayerStack m_LayerStack;

	private:
		static inline Application* s_Instance = nullptr;
		friend int AppMain(int argc, char** argv);

	private:
		void OnWindowClose(const Events::WindowCloseEvent& e);
		EventHandler<Events::WindowCloseEvent> m_WindowCloseCallback;
	};


	// Needs to be defined/implemented in Client/Application
	Application* CreateApplication(Application::CommandLineArgs args);


} // namespace Helios::Engine
