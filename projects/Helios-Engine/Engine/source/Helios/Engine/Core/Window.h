#pragma once

#include "Helios/Engine/Events/Event.h"
//#include "Helios/Engine/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Helios::Engine {


	class Window
	{
	public:
		struct Specification
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;

			Specification(const std::string& title = "HeliosEngine - Default Title", uint32_t width = 800, uint32_t height = 600)
				: Title(title), Width(width), Height(height) {}
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
		void Init(const Specification& spec);
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
