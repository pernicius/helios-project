#pragma once

#include "Helios/Engine/Core/LayerStack.h"
#include "Helios/Engine/Events/Event.h"
#include "Helios/Engine/Events/Types/Window.h"
#include "Helios/Engine/Renderer/Window.h"

#include <mutex>
#include <memory>

namespace Helios::Engine {


	int AppMain(int argc, char** argv);


	class Application
	{
	public:
		Application();
		~Application();

		static Application& Get() { return *s_Instance; }
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
	Application* CreateApplication();


} // namespace Helios::Engine
