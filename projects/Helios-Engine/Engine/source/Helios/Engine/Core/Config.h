#pragma once

namespace Helios::Engine {


	class Config
	{
	public:
		static bool Read(const std::string file, const std::string path);
		static bool Save();

		static std::string Get(const std::string& key, const std::string& default_value = "", const std::string& opt_descr = "");
		static void Set(const std::string& key, const std::string& value, const std::string& opt_descr = "");
		static void Override(const std::string& key, const std::string& value);

	private:
		static std::unordered_map<std::string, std::string> m_keyval;
		static std::unordered_map<std::string, std::string> m_keyval_overrides;
		static std::unordered_map<std::string, std::string> m_keydescr;
		static std::string m_file;

	private:
		Config() = delete;
		~Config() = delete;
	};


} //namespace Helios::Engine
