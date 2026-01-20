#pragma once

namespace Helios::Engine {


	class AppSpec
	{
	public:
		// Name of Application
		std::string Name = "AppName";

		// Version of Application
		uint32_t Version = 0;
		
		// Base path for FileIO
		std::string WorkingDirectory = ".";
		
		// Command Line Arguments
		struct CommandLineArgs
		{
			int Count = 0;
			char** Args = nullptr;

			const char* operator[](int index) const {
				return Args[index];
			}

			bool Check(std::string_view arg) const;
			std::string Get(std::string_view arg, std::string_view default_value = {}) const;
		} CmdLineArgs;
		
		// Basic configuration hints
		unsigned int Hints = 0;
		enum HintFlags
		{
			USE_CWD     = (1 << 0), // use the current work dir as base path
			USE_EXEPATH = (1 << 1), // use path of executeable as base path
		};
		
		// Filename of the logfile
		std::string LogFile = "log.log";
		
		// Filename of the configfile
		std::string ConfigFile = "config.cfg";
	};


} // namespace Helios::Engine
