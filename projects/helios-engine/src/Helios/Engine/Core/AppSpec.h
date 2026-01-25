//==============================================================================
// Application Specification
//
// Defines the configuration structure for initializing Helios Engine
// applications. Contains application metadata (name, version), filesystem
// paths (working directory, log file, config file), command-line argument
// handling, and initialization hints for engine behavior.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
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
