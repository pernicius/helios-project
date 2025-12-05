#include "pch.h"
#include "Helios/Engine/Renderer/Window.h"

#include "Helios/Engine/Renderer/RendererAPI.h"

#include "Helios/Engine/Core/Application.h"
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

#include <memory>

namespace Helios::Engine::Renderer {


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


	void Window::RaiseEvent(Scope<Event> event)
	{
		Application::Get().SubmitEvent(std::move(event));
	}


	void Window::InitCallbacks()
	{
		glfwSetWindowUserPointer(m_Window, this);

		// glfwSetWindowPosCallback

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* handle, int width, int height) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<WindowResizeEvent>(width, height));
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* handle) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<WindowCloseEvent>());
		});

		// glfwSetWindowRefreshCallback
		// glfwSetWindowFocusCallback
		// glfwSetWindowIconifyCallback
		// glfwSetWindowMaximizeCallback

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* handle, int width, int height) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<FramebufferResizeEvent>(width, height));
		});

		// glfwSetWindowContentScaleCallback
		
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* handle, int button, int action, int mods)
		{
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			switch (action)
			{
				case GLFW_PRESS:
					window->RaiseEvent(CreateScope<MouseButtonPressedEvent>(button));
					break;
				case GLFW_RELEASE:
					window->RaiseEvent(CreateScope<MouseButtonReleasedEvent>(button));
					break;
			}
		});
		
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* handle, double xPos, double yPos) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<MouseMovedEvent>(static_cast<float>(xPos), static_cast<float>(yPos)));
		});
		
		// glfwSetCursorEnterCallback
		
		glfwSetScrollCallback(m_Window, [](GLFWwindow* handle, double xOffset, double yOffset) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<MouseScrolledEvent>(static_cast<float>(xOffset), static_cast<float>(yOffset)));
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* handle, int key, int scancode, int action, int mode)
		{
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			switch (action)
			{
				case GLFW_PRESS:
					window->RaiseEvent(CreateScope<KeyPressedEvent>(key, 0));
					break;
				case GLFW_RELEASE:
					window->RaiseEvent(CreateScope<KeyReleasedEvent>(key));
					break;
				case GLFW_REPEAT:
					window->RaiseEvent(CreateScope<KeyPressedEvent>(key, 1));
					break;
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* handle, unsigned int keycode) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
			if (!window) return;
			window->RaiseEvent(CreateScope<KeyTypedEvent>(keycode));
		});

		// glfwSetDropCallback
		// glfwSetMonitorCallback
		// glfwSetJoystickCallback
	}


} // namespace Helios::Engine::Renderer
