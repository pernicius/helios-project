#include "pch.h"
#include "Platform/Renderer/OpenGL/GLSupport.h"

#include "Helios/Engine/Core/Log.h"

#include <glfw/glfw3.h>
#include <glad/gl.h>

#include <mutex>

namespace Helios::Engine::OpenGL
{
	bool IsSupported()
	{


		// TODO: some questions for github-copilot:
		//
		// which are the most common required opengl extensions for game engines
		// What are some best practices for checking and managing OpenGL extensions in a game engine?
		// show me an example of an OpenGLExtensionManager



		static std::once_flag s_flag;
		static bool s_result = false;

		std::call_once(s_flag, []()
		{
			GLFWerrorfun previousCallback = glfwSetErrorCallback(nullptr);

			if (!glfwInit()) {
				s_result = false;
				return;
			}

			glfwDefaultWindowHints();
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
//			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

			GLFWwindow* window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
			if (!window) {
				glfwTerminate();
				glfwSetErrorCallback(previousCallback);
				s_result = false;
				LOG_RENDER_INFO("OpenGL is not supported.");
				return;
			}

			glfwMakeContextCurrent(window);
			int version = gladLoadGL(glfwGetProcAddress);
			int major = GLAD_VERSION_MAJOR(version);
			int minor = GLAD_VERSION_MINOR(version);
			LOG_RENDER_INFO("OpenGL {}.{} Info:", major, minor);
			LOG_RENDER_INFO("  Vendor:   {0}", (char*)(glGetString(GL_VENDOR)));
			LOG_RENDER_INFO("  Renderer: {0}", (char*)(glGetString(GL_RENDERER)));
			LOG_RENDER_INFO("  Version:  {0}", (char*)(glGetString(GL_VERSION)));
			LOG_RENDER_INFO("  Shader:   {0}", (char*)(glGetString(GL_SHADING_LANGUAGE_VERSION)));

			glfwDestroyWindow(window);
			glfwSetErrorCallback(previousCallback);
			glfwTerminate();

			// Check for minimum version
			if (major < 4 || (major == 4 && minor < 5)) {
				s_result = false;
				LOG_RENDER_INFO("OpenGL is not supported.");
				return;
			}

			// Check for required extensions
			//   TODO: which extensions are required?
			
			// Check for optional extensions
			//   TODO: which extensions are optional?

			LOG_RENDER_INFO("OpenGL is fully/partialy supported.");
			s_result = true;
		});

		return s_result;
	}
} // namespace Helios::Engine::OpenGL
