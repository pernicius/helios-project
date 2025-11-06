#include "pch.h"
#include "Helios/Engine/Renderer/Window.h"

#include "Helios/Engine/Core/Log.h"
#include "Helios/Engine/Renderer/RendererAPI.h"

#include "Helios/Engine/Events/EventManager.h"
#include "Helios/Engine/Events/Types/Key.h"
#include "Helios/Engine/Events/Types/Mouse.h"
#include "Helios/Engine/Events/Types/Window.h"

#ifdef HE_RENDERER_OPENGL
#	include "Platform/Renderer/OpenGL/GLWindow.h"
#endif
#ifdef HE_RENDERER_VULKAN
#	include "Platform/Renderer/Vulkan/VKWindow.h"
#endif
#ifdef HE_RENDERER_DIRECTX
#	include "Platform/Renderer/DirectX/DX12Window.h"
#endif
#ifdef HE_RENDERER_METAL
#	include "Platform/Renderer/Metal/MTWindow.h"
#endif

namespace Helios::Engine {


	Scope<Window> Window::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			LOG_RENDER_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;

#		ifdef HE_RENDERER_OPENGL
			case RendererAPI::API::OpenGL: return CreateScope<GLWindow>();
#		endif
#		ifdef HE_RENDERER_VULKAN
			case RendererAPI::API::Vulkan: return CreateScope<VKWindow>();
#		endif
#		ifdef HE_RENDERER_DIRECTX
			case RendererAPI::API::DirectX: return CreateScope<DXWindow>();
#		endif
#		ifdef HE_RENDERER_METAL
			case RendererAPI::API::Metal: return CreateScope<MTWindow>();
#		endif
		}

		LOG_RENDER_ERROR("Unknown RendererAPI!");
		return nullptr;
	}


	void Window::InitCallbacks()
	{
		// glfwSetWindowPosCallback

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
//			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//			data.Width = width;
//			data.Height = height;
			Events::TriggerEvent(Events::WindowResizeEvent(width, height));
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			Scope<Events::WindowCloseEvent> closeEvent = CreateScope<Events::WindowCloseEvent>();
			Events::QueueEvent(std::move(closeEvent));
			});

		// glfwSetWindowRefreshCallback
		// glfwSetWindowFocusCallback
		// glfwSetWindowIconifyCallback
		// glfwSetWindowMaximizeCallback
		// glfwSetFramebufferSizeCallback
		// glfwSetWindowContentScaleCallback
		
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			switch (action)
			{
			case GLFW_PRESS:
				Events::TriggerEvent(Events::MouseButtonPressedEvent(button));
				break;
			case GLFW_RELEASE:
				Events::TriggerEvent(Events::MouseButtonReleasedEvent(button));
				break;
			}
			});
		
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
			Events::TriggerEvent(Events::MouseMovedEvent((float)xPos, (float)yPos));
			});
		
		// glfwSetCursorEnterCallback
		
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
			Events::TriggerEvent(Events::MouseScrolledEvent((float)xOffset, (float)yOffset));
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mode) {
			switch (action)
			{
			case GLFW_PRESS:
				Events::TriggerEvent(Events::KeyPressedEvent(key, 0));
				break;
			case GLFW_RELEASE:
				Events::TriggerEvent(Events::KeyReleasedEvent(key));
				break;
			case GLFW_REPEAT:
				Events::TriggerEvent(Events::KeyPressedEvent(key, 1));
				break;
			}
			});
		
		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
			Scope<Events::KeyTypedEvent> keyTypeEvent = CreateScope<Events::KeyTypedEvent>(keycode);
			Events::QueueEvent(std::move(keyTypeEvent));
			});

		// glfwSetDropCallback
		// glfwSetMonitorCallback
		// glfwSetJoystickCallback
	}


}// namespace Helios::Engine
