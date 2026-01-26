//==============================================================================
// Application Core
//
// Provides the central application management system for the Helios Engine.
// Handles application lifecycle (initialization, main loop, shutdown), window
// creation, event processing and dispatch, layer stack management, and
// renderer initialization. Implements a singleton pattern with entry point
// integration through AppMain.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - Singleton application management with lifecycle control
// - Thread-safe event queue with dispatch to layer stack
// - Layer/overlay management system (push/pop operations)
// - Window creation and event handling integration
// - Renderer initialization coordination
// - Application restart support
// - Configuration management via AppSpec
// - Entry point integration (AppMain) and factory pattern
// 
// Version history:
// - 2026.01: Added use of ConfigManager
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include "Helios/Engine/Core/AppSpec.h"

#include "Helios/Engine/Core/LayerStack.h"
#include "Helios/Engine/Core/Events.h"
#include "Helios/Engine/Core/EventTypeWindow.h"

#include "Helios/Engine/Renderer/Window.h"
//#include "Helios/Engine/Renderer/DeviceManager.h"

namespace Helios::Engine {


	int AppMain(int argc, char** argv);


	class Application
	{
	public:
		Application() = delete;
		Application(const AppSpec& spec);
		~Application();

		static Application& Get() { return *s_Instance; }
		AppSpec& GetAppSpec() { return m_Spec; }
		bool NeedRestart(bool setRestart = false);

		void PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }
		void PopLayer(Layer* layer) { m_LayerStack.PopLayer(layer); }
		void PushOverlay(Layer* layer) { m_LayerStack.PushOverlay(layer); }
		void PopOverlay(Layer* layer) { m_LayerStack.PopOverlay(layer); }

		void SubmitEvent(Scope<Event> event);
		void ProcessEvents();
		void OnEvent(Event& e);

		virtual void OnInit() {}
		virtual void OnShutdown() {}

		void InitRenderer();
//		Ref<Renderer::Window>& GetAppWindow() { return m_Window; }
//		Ref<Renderer::DeviceManager>& GetDeviceManager() { return m_DeviceManager; }

	private:
		void Run();

	private:
		AppSpec m_Spec;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		std::mutex m_EventQueueMutex;
		std::vector<Scope<Event>> m_EventQueue;

		Ref<Renderer::Window> m_Window;
//		Ref<Renderer::DeviceManager> m_DeviceManager;

	private:
		static inline Application* s_Instance = nullptr;
		friend int AppMain(int argc, char** argv);

	private:
		bool OnWindowClose(const WindowCloseEvent& e);
//		bool OnWindowResize(const WindowResizeEvent& e);
//		bool OnFramebufferResize(const FramebufferResizeEvent& e);
	};


	// Needs to be defined/implemented in Client/Application
	Application* CreateApplication(int argc, char** argv);


} // namespace Helios::Engine
