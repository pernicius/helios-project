#include "pch.h"
#include "Log.h"

#include <filesystem>

#pragma warning(push, 0)
#	include <spdlog/spdlog.h>
#	include <spdlog/fmt/ostr.h>
#	include <spdlog/sinks/stdout_color_sinks.h>
#	include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

#include <GLFW/glfw3.h>

namespace Helios::Engine {


//	Ref<spdlog::logger> Log::s_CoreLogger;
//	Ref<spdlog::logger> Log::s_GLFWLogger;
//	Ref<spdlog::logger> Log::s_AppLogger;
//	Ref<spdlog::logger> Log::s_RenderLogger;


	void GLFW_error_callback(int code, const char* description)
	{
		LOG_GLFW_ERROR("ErrorCallback({}) {}", code, description);
	}


	void Log::Init(const std::string& filename, const std::string& path)
	{
		std::vector<spdlog::sink_ptr> logSinks;

		std::string file = std::filesystem::path(
			(path.empty() ? "" : path + "/") + (filename.empty() ? "logger.log" : filename)
		).make_preferred().string();

		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(file, true));

//		logSinks[0]->set_pattern("%T.%e [P.%P:T.%t] %4n:%-5!l >> %^%v%$");
//		logSinks[1]->set_pattern("%T.%e [P.%P:T.%t] %4n:%-5!l >> %v");
		logSinks[0]->set_pattern("%T %4n:%-5!l >> %^%v%$");
		logSinks[1]->set_pattern("%T %4n:%-5!l >> %v");

		// Logger for the HeliosEngine
		s_CoreLogger = std::make_shared<spdlog::logger>("CORE", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);
		LOG_CORE_DEBUG("Log initialized");

		// Logger for GLFW
		s_GLFWLogger = std::make_shared<spdlog::logger>("GLFW", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_GLFWLogger);
		s_GLFWLogger->set_level(spdlog::level::trace);
		s_GLFWLogger->flush_on(spdlog::level::trace);
		LOG_GLFW_DEBUG("Log initialized");

		glfwSetErrorCallback(GLFW_error_callback);

		// Logger for the Renderer
		s_RenderLogger = std::make_shared<spdlog::logger>("REND", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_RenderLogger);
		s_RenderLogger->set_level(spdlog::level::trace);
		s_RenderLogger->flush_on(spdlog::level::trace);
		LOG_RENDER_DEBUG("Log initialized");

		// Logger for the Application
		s_AppLogger = std::make_shared<spdlog::logger>("APP ", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_AppLogger);
		s_AppLogger->set_level(spdlog::level::trace);
		s_AppLogger->flush_on(spdlog::level::trace);
		LOG_DEBUG("Log initialized");
	}


} // namespace Helios::Engine
