#include "pch.h"
#include "Helios/Engine/Core/Log.h"

#pragma warning(push, 0)
#	include <spdlog/sinks/stdout_color_sinks.h>
//#	include <spdlog/sinks/basic_file_sink.h>
#	include <spdlog/sinks/rotating_file_sink.h>
#	include <spdlog/sinks/null_sink.h>
#pragma warning(pop)

#include <GLFW/glfw3.h>

namespace Helios::Engine {


	void GLFW_error_callback(int code, const char* description)
	{
		LOG_GLFW_ERROR("ErrorCallback({}) {}", code, description);
	}


	// Helper that creates a no-op logger (does not register it with spdlog registry).
	static Ref<spdlog::logger> CreateNoopLogger(const std::string& name)
	{
		auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
		auto logger = std::make_shared<spdlog::logger>(name, sink);
		logger->set_level(spdlog::level::off);
		// Do NOT register this logger with the spdlog registry; keep it local so it remains valid even if spdlog::shutdown() is called.
		return logger;
	}


	// ----------------------------------------------------------------------------------------------------


	void Log::Init(const std::string& filename, const std::string& path)
	{
		std::vector<spdlog::sink_ptr> logSinks;

		std::string file = std::filesystem::path(
			(path.empty() ? "" : path + "/") + (filename.empty() ? "logger.log" : filename)
		).make_preferred().string();

		// console logger
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		// file logger
//		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(file, true));
		// rotating file logger (10 Mbyte, 3 files)
		logSinks.emplace_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(file, 1048575*10, 3, true));

//		logSinks[0]->set_pattern("%T.%e [P.%P:T.%t] %4n:%-5!l >> %^%v%$");
//		logSinks[1]->set_pattern("%T.%e [P.%P:T.%t] %4n:%-5!l >> %v");
		logSinks[0]->set_pattern("%T %4n:%-5!l >> %^%v%$");
		logSinks[1]->set_pattern("%T %4n:%-5!l >> %v");

		// Logger for the Engine
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

		// Setup the error callback for GLFW
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


	void Log::Shutdown()
	{
		// Unregister GLFW error callback to avoid it invoking logging while we're tearing down
		glfwSetErrorCallback(nullptr);

		LOG_CORE_INFO("Logging stopped.");

		// flush existing loggers
		if (s_CoreLogger)   s_CoreLogger->flush();
		if (s_GLFWLogger)   s_GLFWLogger->flush();
		if (s_RenderLogger) s_RenderLogger->flush();
		if (s_AppLogger)    s_AppLogger->flush();

		// remove named loggers from spdlog registry (optional but explicit)
		try {
			spdlog::drop("CORE");
			spdlog::drop("GLFW");
			spdlog::drop("REND");
			spdlog::drop("APP ");
		}
		catch (...) {
			// ignore any drop errors during shutdown
		}

		// global shutdown: releases sinks and clears registry
		spdlog::shutdown();

		// Replace our references with no-op loggers so logging macros stay safe between Shutdown() and the next Init().
		// These no-op loggers hold their own null sinks and are NOT registered with spdlog's global registry.
		s_CoreLogger   = CreateNoopLogger("CORE_NOOP");
		s_GLFWLogger   = CreateNoopLogger("GLFW_NOOP");
		s_RenderLogger = CreateNoopLogger("REND_NOOP");
		s_AppLogger    = CreateNoopLogger("APP_NOOP");
	}


} // namespace Helios::Engine
