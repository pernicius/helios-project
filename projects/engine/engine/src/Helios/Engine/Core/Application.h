#pragma once

#include "Helios/Engine/Core/Log.h"
#include "Helios/Engine/Core/LayerStack.h"
#include "Helios/Engine/Events/Event.h"
#include "Helios/Engine/Events/Types/Window.h"
#include "Helios/Engine/Renderer/Window.h"

#include <string>
#include <string_view>
#include <cstdint>
#include <mutex>
#include <vector>
#include <memory>

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

		void PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }
		void PopLayer(Layer* layer) { m_LayerStack.PopLayer(layer); }
		void PushOverlay(Layer* layer) { m_LayerStack.PushOverlay(layer); }
		void PopOverlay(Layer* layer) { m_LayerStack.PopOverlay(layer); }

		void SubmitEvent(Scope<Event> event);
		void ProcessEvents();
		void OnEvent(Event& e);

	private:
		void Run();

	private:
		Scope<Window> m_Window;
		Specification m_Spec;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		std::mutex m_EventQueueMutex;
		std::vector<Scope<Event>> m_EventQueue;

	private:
		static inline Application* s_Instance = nullptr;
		friend int AppMain(int argc, char** argv);

	private:
		bool OnWindowClose(const WindowCloseEvent& e);
//		bool OnWindowResize(const WindowResizeEvent& e);
//		bool OnFramebufferResize(const FramebufferResizeEvent& e);
	};


	// Needs to be defined/implemented in Client/Application
	Application* CreateApplication(Application::CommandLineArgs args);


} // namespace Helios::Engine
