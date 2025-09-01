#pragma once

#include "Helios/Engine/Events/Event.h"
//#include "Helios/Engine/Renderer/GraphicsContext.h"
//#include "Helios/Engine/Renderer/RendererSpec.h"

#include <GLFW/glfw3.h>

namespace Helios::Engine {


	class Window
	{
	public:
		struct Specification
		{
			std::string Title = "HeliosEngine - Default Title";

			uint32_t Width = 800;
			uint32_t Height = 600;
		};
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		static Scope<Window> Create(const Specification& spec = Specification());
		Window(const Specification& spec);
		~Window();

		void OnUpdate();

		unsigned int GetWidth() { return m_Data.Width; }
		unsigned int GetHeight() { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled);
		bool IsVSync() const;

		void* GetNativeWindow() const { return m_Window; }

	private:

		void InitCallbacks();
		void Shutdown();

	private:
		GLFWwindow* m_Window;
//		Scope<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width = 0;
			unsigned int Height = 0;
			bool VSync = true;
			EventCallbackFn EventCallback;
		};
		WindowData m_Data;
	};


} // namespace Helios::Engine
