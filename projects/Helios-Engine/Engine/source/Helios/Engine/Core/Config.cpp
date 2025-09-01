#include "pch.h"
#include "Helios/Engine/Core/Config.h"

namespace Helios::Engine {


	std::unordered_map<std::string, std::string> Config::m_keyval = {};
	std::unordered_map<std::string, std::string> Config::m_keyval_overrides = {};
	std::unordered_map<std::string, std::string> Config::m_keydescr = {};
	std::string Config::m_file;


	bool Config::Read(const std::string file, const std::string path)
	{
		// generate file path+name
		m_file = std::filesystem::path(
			(path.empty() ? "" : path + "/") + (file.empty() ? "config.cfg" : file)
		).make_preferred().string();
		LOG_CORE_DEBUG("Reading config file '{}'", std::filesystem::path(m_file).filename().string());

		// open file
		std::ifstream isfile;
		isfile.open(m_file, std::ifstream::in);
		if (!isfile.is_open())
			return false;
		
		// read each line
		std::string line;
		while (!isfile.eof())
		{
			std::getline(isfile, line);
			if (line.length() > 0)
			{
				std::string key = line.substr(0, line.find_first_of('='));
				std::string value = line.substr(line.find_first_of('=') + 1);

				if (key.length() > 0 and value.length() > 0 and line.find_first_of('=') != std::string::npos)
					m_keyval[key] = value;
			}
		}
		isfile.close();

		return true;
	}


	bool Config::Save()
	{
		LOG_CORE_DEBUG("Saving config file '{}'", std::filesystem::path(m_file).filename().string());

		// TODO
		// TODO
		// TODO

		return false;
	}


	std::string Config::Get(const std::string& key, const std::string& default_value, const std::string& opt_descr)
	{
		if (m_keyval_overrides.contains(key))
			return m_keyval_overrides[key];
		
		if (m_keyval.contains(key))
			return m_keyval[key];
		
		Set(key, default_value, opt_descr);
		return default_value;
	}


	void Config::Set(const std::string& key, const std::string& value, const std::string& opt_descr)
	{
		m_keyval[key] = value;
		if (!opt_descr.empty())
			m_keydescr[key] = opt_descr;
	}


	void Config::Override(const std::string& key, const std::string& value)
	{
		m_keyval_overrides[key] = value;
	}


} //namespace Helios::Engine
