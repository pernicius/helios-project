#pragma once

namespace Helios::Engine::Spec {


	class App
	{
	public:
		// Name of Application
		static std::string Name;

		// Version of Application
		static uint32_t Version;
		
		// Base path for FileIO
		static std::string WorkingDirectory;
		
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
		};
		static CommandLineArgs CmdLineArgs;
		
		// Basic configuration hints
		static unsigned int Hints;
		enum HintFlags
		{
			USE_CWD     = (1 << 0), // use the current work dir as base path
			USE_EXEPATH = (1 << 1), // use path of executeable as base path
		};
		
		// Filename of the logfile
		static std::string LogFile;
		
		// Filename of the configfile
		static std::string ConfigFile;
	};


} // namespace Helios::Engine::Spec
