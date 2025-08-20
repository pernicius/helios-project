#pragma once

#pragma warning(push, 0)
#	include <spdlog/spdlog.h>
//#	include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include "Helios/Engine/Core/ScopeRef.h"
#include "Helios/Engine/Debug/DebugBreak.h"


namespace Helios {


	class Log
	{
	public:
		static void Init(const std::string& filename = "HeliosEngine.log", const std::string& path = "");

		static Ref<spdlog::logger>& GetCoreLogger()   { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetGLFWLogger()   { return s_GLFWLogger; }
		static Ref<spdlog::logger>& GetAppLogger()    { return s_AppLogger; }
		static Ref<spdlog::logger>& GetRenderLogger() { return s_RenderLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_GLFWLogger;
		static Ref<spdlog::logger> s_AppLogger;
		static Ref<spdlog::logger> s_RenderLogger;
	};


} // namespace Helios


// log levels
#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO  2
#define LOG_LEVEL_WARN  3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_FATAL 5
#define LOG_LEVEL_OFF   6

// minimal log level
#if !defined(LOG_LEVEL)
#	ifdef BUILD_DEBUG
#		define LOG_LEVEL LOG_LEVEL_TRACE
#	else
#		define LOG_LEVEL LOG_LEVEL_INFO
#	endif
#endif

// Logging macros for HeliosEngine
#define LOG_CORE_TRACE(...)   (LOG_LEVEL <= LOG_LEVEL_TRACE) ? ::Helios::Log::GetCoreLogger()->trace(__VA_ARGS__)      : (void)0
#define LOG_CORE_DEBUG(...)   (LOG_LEVEL <= LOG_LEVEL_DEBUG) ? ::Helios::Log::GetCoreLogger()->debug(__VA_ARGS__)      : (void)0
#define LOG_CORE_INFO(...)    (LOG_LEVEL <= LOG_LEVEL_INFO)  ? ::Helios::Log::GetCoreLogger()->info(__VA_ARGS__)       : (void)0
#define LOG_CORE_WARN(...)    (LOG_LEVEL <= LOG_LEVEL_WARN)  ? ::Helios::Log::GetCoreLogger()->warn(__VA_ARGS__)       : (void)0
#define LOG_CORE_ERROR(...)   (LOG_LEVEL <= LOG_LEVEL_ERROR) ? ::Helios::Log::GetCoreLogger()->error(__VA_ARGS__)      : (void)0
#define LOG_CORE_FATAL(...)   (LOG_LEVEL <= LOG_LEVEL_FATAL) ? ::Helios::Log::GetCoreLogger()->critical(__VA_ARGS__)   : (void)0

// Logging macros for Application
#define LOG_TRACE(...)        (LOG_LEVEL <= LOG_LEVEL_TRACE) ? ::Helios::Log::GetAppLogger()->trace(__VA_ARGS__)       : (void)0
#define LOG_DEBUG(...)        (LOG_LEVEL <= LOG_LEVEL_DEBUG) ? ::Helios::Log::GetAppLogger()->debug(__VA_ARGS__)       : (void)0
#define LOG_INFO(...)         (LOG_LEVEL <= LOG_LEVEL_INFO)  ? ::Helios::Log::GetAppLogger()->info(__VA_ARGS__)        : (void)0
#define LOG_WARN(...)         (LOG_LEVEL <= LOG_LEVEL_WARN)  ? ::Helios::Log::GetAppLogger()->warn(__VA_ARGS__)        : (void)0
#define LOG_ERROR(...)        (LOG_LEVEL <= LOG_LEVEL_ERROR) ? ::Helios::Log::GetAppLogger()->error(__VA_ARGS__)       : (void)0
#define LOG_FATAL(...)        (LOG_LEVEL <= LOG_LEVEL_FATAL) ? ::Helios::Log::GetAppLogger()->critical(__VA_ARGS__)    : (void)0

// Logging macros for GLFW
#define LOG_GLFW_DEBUG(...)   (LOG_LEVEL <= LOG_LEVEL_DEBUG) ? ::Helios::Log::GetGLFWLogger()->debug(__VA_ARGS__)      : (void)0
#define LOG_GLFW_ERROR(...)   (LOG_LEVEL <= LOG_LEVEL_ERROR) ? ::Helios::Log::GetGLFWLogger()->error(__VA_ARGS__)      : (void)0
#define LOG_GLFW_FATAL(...)   (LOG_LEVEL <= LOG_LEVEL_FATAL) ? ::Helios::Log::GetGLFWLogger()->critical(__VA_ARGS__)   : (void)0

// Logging macros for Renderer
#define LOG_RENDER_TRACE(...) (LOG_LEVEL <= LOG_LEVEL_TRACE) ? ::Helios::Log::GetRenderLogger()->trace(__VA_ARGS__)    : (void)0
#define LOG_RENDER_DEBUG(...) (LOG_LEVEL <= LOG_LEVEL_DEBUG) ? ::Helios::Log::GetRenderLogger()->debug(__VA_ARGS__)    : (void)0
#define LOG_RENDER_INFO(...)  (LOG_LEVEL <= LOG_LEVEL_INFO)  ? ::Helios::Log::GetRenderLogger()->info(__VA_ARGS__)     : (void)0
#define LOG_RENDER_WARN(...)  (LOG_LEVEL <= LOG_LEVEL_WARN)  ? ::Helios::Log::GetRenderLogger()->warn(__VA_ARGS__)     : (void)0
#define LOG_RENDER_ERROR(...) (LOG_LEVEL <= LOG_LEVEL_ERROR) ? ::Helios::Log::GetRenderLogger()->error(__VA_ARGS__)    : (void)0
#define LOG_RENDER_FATAL(...) (LOG_LEVEL <= LOG_LEVEL_FATAL) ? ::Helios::Log::GetRenderLogger()->critical(__VA_ARGS__) : (void)0

// ASSERT macros
#if HE_LOG_ASSERTS_ENABLED
#	define LOG_CORE_ASSERT(x, ...)   { if(!(x)) { LOG_CORE_FATAL(  "Assertion failed: {0}", __VA_ARGS__); DebugBreak(); } }
#	define LOG_GLFW_ASSERT(x, ...)   { if(!(x)) { LOG_GLFW_FATAL(  "Assertion failed: {0}", __VA_ARGS__); DebugBreak(); } }
#	define LOG_RENDER_ASSERT(x, ...) { if(!(x)) { LOG_RENDER_FATAL("Assertion failed: {0}", __VA_ARGS__); DebugBreak(); } }
#	define LOG_ASSERT(x, ...)        { if(!(x)) { LOG_FATAL(       "Assertion failed: {0}", __VA_ARGS__); DebugBreak(); } }
#else
#	define LOG_CORE_ASSERT(x, ...)
#	define LOG_GLFW_ASSERT(x, ...)
#	define LOG_RENDER_ASSERT(x, ...)
#	define LOG_ASSERT(x, ...)
#endif

// EXCEPTION macros (exceptions are always logged as fatal errors)
#define LOG_CORE_EXCEPT(x)   { LOG_CORE_FATAL(  "Core-Exception: {}",     x); throw std::runtime_error(x); }
#define LOG_GLFW_EXCEPT(x)   { LOG_GLFW_FATAL(  "GLFW-Exception: {}",     x); throw std::runtime_error(x); }
#define LOG_RENDER_EXCEPT(x) { LOG_RENDER_FATAL("Renderer-Exception: {}", x); throw std::runtime_error(x); }
#define LOG_EXCEPT(x)        { LOG_FATAL(       "App-Exception: {}",      x); throw std::runtime_error(x); }
