//==============================================================================
// Configuration Management System (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file Config.h
//==============================================================================
#include "pch.h"
#include "Helios/Engine/Core/Config.h"

#include "Helios/Engine/Util/IniParser.h"

#include <sstream>
#include <limits>
#include <charconv>

namespace Helios::Engine::Core {


	//------------------------------------------------------------------------------
	// ConfigSection Implementation
	//------------------------------------------------------------------------------


	bool ConfigSection::HasKey(const std::string& key) const
	{
		std::shared_lock lock(m_mutex);
		return m_data.find(key) != m_data.end();
	}


	template<typename T>
	T ConfigSection::Get(const std::string& key, const T& defaultValue) const
	{
		std::shared_lock lock(m_mutex);

		auto it = m_data.find(key);
		if (it == m_data.end())
			return defaultValue;

		T result{};
		if (ParseValue(it->second, result))
			return result;

		return defaultValue;
	}


	template<typename T>
	void ConfigSection::Set(const std::string& key, const T& value)
	{
		std::unique_lock lock(m_mutex);
		m_data[key] = StringifyValue(value);
	}


	std::string ConfigSection::GetKeyComment(const std::string& key) const
	{
		std::shared_lock lock(m_mutex);
		auto it = m_keyComments.find(key);
		if (it != m_keyComments.end())
			return it->second;
		return "";
	}


	// Helper: Parse string to typed value
	template<typename T>
	bool ConfigSection::ParseValue(const std::string& str, T& out)
	{
		if constexpr (std::is_same_v<T, std::string>) {
			out = str;
			return true;
		}
		else if constexpr (std::is_same_v<T, bool>) {
			std::string lower = str;
			std::transform(lower.begin(), lower.end(), lower.begin(), 
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			
			if (lower == "true" || lower == "1" || lower == "yes" || lower == "on" || lower == "y") {
				out = true;
				return true;
			}
			if (lower == "false" || lower == "0" || lower == "no" || lower == "off" || lower == "n") {
				out = false;
				return true;
			}
			return false;
		}
		else if constexpr (std::is_integral_v<T>) {
			if (str.empty())
				return false;
			const char* begin = str.c_str();
			const char* end = begin + str.size();
			auto result = std::from_chars(begin, end, out);
			return result.ec == std::errc{} && result.ptr == end;
		}
		else if constexpr (std::is_floating_point_v<T>) {
			if (str.empty())
				return false;
			char* endptr = nullptr;
			errno = 0;
			const char* cstr = str.c_str();
			double temp = std::strtod(cstr, &endptr);
			if (endptr == cstr || errno == ERANGE)
				return false;
			while (*endptr != '\0') {
				if (!std::isspace(static_cast<unsigned char>(*endptr)))
					return false;
				++endptr;
			}
			out = static_cast<T>(temp);
			return true;
		}
		else {
			static_assert(always_false<T>, "ConfigSection::ParseValue unsupported type");
			return false;
		}
	}


	// Helper: Convert typed value to string
	template<typename T>
	std::string ConfigSection::StringifyValue(const T& value)
	{
		if constexpr (std::is_same_v<T, std::string>) {
			return value;
		}
		else if constexpr (std::is_same_v<T, const char*>) {
			return std::string(value);
		}
		else if constexpr (std::is_same_v<T, bool>) {
			return value ? "true" : "false";
		}
		else if constexpr (std::is_integral_v<T>) {
			return std::to_string(value);
		}
		else if constexpr (std::is_floating_point_v<T>) {
			std::ostringstream ss;
			ss.precision(std::numeric_limits<T>::digits10 + 1);
			ss << value;
			return ss.str();
		}
		else {
			static_assert(always_false<T>, "ConfigSection::StringifyValue unsupported type");
			return std::string{};
		}
	}


	// Explicit template instantiations for common types
	template std::string ConfigSection::Get<std::string>(const std::string&, const std::string&) const;
	template bool ConfigSection::Get<bool>(const std::string&, const bool&) const;
	template int ConfigSection::Get<int>(const std::string&, const int&) const;
	template float ConfigSection::Get<float>(const std::string&, const float&) const;
	template double ConfigSection::Get<double>(const std::string&, const double&) const;

	template void ConfigSection::Set<std::string>(const std::string&, const std::string&);
	template void ConfigSection::Set<bool>(const std::string&, const bool&);
	template void ConfigSection::Set<int>(const std::string&, const int&);
	template void ConfigSection::Set<float>(const std::string&, const float&);
	template void ConfigSection::Set<double>(const std::string&, const double&);


	//------------------------------------------------------------------------------
	// ConfigDomain Implementation
	//------------------------------------------------------------------------------


	bool ConfigDomain::Load(const std::string& filePath)
	{
		std::unique_lock lock(m_mutex);

		if (!filePath.empty())
			m_filePath = filePath;

		Helios::Util::IniParser parser;
		if (!parser.Load(m_filePath))
			return false;

		m_sections.clear();
		
		// Load data
		for (const auto& sectionName : parser.Sections()) {
			for (const auto& key : parser.Keys(sectionName)) {
				std::string value = parser.Get<std::string>(sectionName, key, "", true);
				m_sections[sectionName].Set<std::string>(key, value);
			}
		}
		
		// Load domain comment
		m_domainComment = parser.GetFileComment();
		
		// Load section and key comments
		for (const auto& sectionName : parser.Sections()) {
			m_sections[sectionName].SetSectionComment(parser.GetSectionComment(sectionName));
			
			for (const auto& key : parser.Keys(sectionName)) {
				std::string comment = parser.GetKeyComment(sectionName, key);
				if (!comment.empty())
					m_sections[sectionName].SetKeyComment(key, comment);
			}
		}
		
		return true;
	}


	bool ConfigDomain::Save(const std::string& filePath) const
	{
		std::shared_lock lock(m_mutex);
		
		std::string savePath = filePath.empty() ? m_filePath : filePath;

		Helios::Util::IniParser parser;
		
		// Save domain comment
		parser.SetFileComment(m_domainComment);
	
		// Copy data and comments from sections to parser
		for (const auto& [sectionName, section] : m_sections) {
			// Save section comment
			parser.SetSectionComment(sectionName, section.GetSectionComment());
		
			// Save data
			std::shared_lock secLock(section.m_mutex);
			for (const auto& [key, value] : section.m_data) {
				parser.Set<std::string>(sectionName, key, value);
			}
		
			// Save key comments
			for (const auto& [key, comment] : section.m_keyComments) {
				parser.SetKeyComment(sectionName, key, comment);
			}
		}

		return parser.Save(savePath);
	}


	ConfigSection& ConfigDomain::GetSection(const std::string& name)
	{
		std::unique_lock lock(m_mutex);
		return m_sections[name];
	}


	const ConfigSection* ConfigDomain::GetSection(const std::string& name) const
	{
		std::shared_lock lock(m_mutex);
		auto it = m_sections.find(name);
		if (it == m_sections.end())
			return nullptr;
		return &it->second;
	}


	bool ConfigDomain::HasSection(const std::string& name) const
	{
		std::shared_lock lock(m_mutex);
		return m_sections.find(name) != m_sections.end();
	}


	template<typename T>
	T ConfigDomain::Get(const std::string& section, const std::string& key, const T& defaultValue) const
	{
		const ConfigSection* sec = GetSection(section);
		if (!sec)
			return defaultValue;
		return sec->Get<T>(key, defaultValue);
	}


	template<typename T>
	void ConfigDomain::Set(const std::string& section, const std::string& key, const T& value)
	{
		GetSection(section).Set<T>(key, value);
	}


	// Explicit template instantiations
	template std::string ConfigDomain::Get<std::string>(const std::string&, const std::string&, const std::string&) const;
	template bool ConfigDomain::Get<bool>(const std::string&, const std::string&, const bool&) const;
	template int ConfigDomain::Get<int>(const std::string&, const std::string&, const int&) const;
	template float ConfigDomain::Get<float>(const std::string&, const std::string&, const float&) const;
	template double ConfigDomain::Get<double>(const std::string&, const std::string&, const double&) const;

	template void ConfigDomain::Set<std::string>(const std::string&, const std::string&, const std::string&);
	template void ConfigDomain::Set<bool>(const std::string&, const std::string&, const bool&);
	template void ConfigDomain::Set<int>(const std::string&, const std::string&, const int&);
	template void ConfigDomain::Set<float>(const std::string&, const std::string&, const float&);
	template void ConfigDomain::Set<double>(const std::string&, const std::string&, const double&);


	std::string ConfigDomain::GetComment(const std::string& section, const std::string& key) const
	{
		if (section.empty())
			return GetDomainComment();

		const ConfigSection* sec = GetSection(section);
		if (!sec)
			return "";

		return sec->GetComment(key);
	}


	std::string ConfigDomain::GetSectionComment(const std::string& section) const
	{
		const ConfigSection* sec = GetSection(section);
		if (!sec)
			return "";
		return sec->GetSectionComment();
	}


	std::string ConfigDomain::GetKeyComment(const std::string& section, const std::string& key) const
	{
		const ConfigSection* sec = GetSection(section);
		if (!sec)
			return "";
		return sec->GetKeyComment(key);
	}


	//------------------------------------------------------------------------------
	// ConfigManager Implementation
	//------------------------------------------------------------------------------


	ConfigManager& ConfigManager::GetInstance()
	{
		static ConfigManager instance;
		return instance;
	}


	bool ConfigManager::LoadDomain(const std::string& domain, const std::string& filePath)
	{
		std::unique_lock lock(m_mutex);
		auto [it, inserted] = m_domains.try_emplace(domain, filePath);
		lock.unlock(); // Unlock before calling Load (which has its own lock)
		return it->second.Load(filePath);
	}


	bool ConfigManager::SaveDomain(const std::string& domain, const std::string& filePath) const
	{
		std::shared_lock lock(m_mutex);
		auto it = m_domains.find(domain);
		if (it == m_domains.end())
			return false;
		
		// Don't hold lock during Save (it has its own lock)
		const ConfigDomain& dom = it->second;
		lock.unlock();
		return dom.Save(filePath);
	}


	ConfigDomain& ConfigManager::GetDomain(const std::string& domain)
	{
		std::unique_lock lock(m_mutex);
		auto [it, inserted] = m_domains.try_emplace(domain, "");
		return it->second;
	}


	const ConfigDomain* ConfigManager::GetDomain(const std::string& domain) const
	{
		std::shared_lock lock(m_mutex);
		auto it = m_domains.find(domain);
		if (it == m_domains.end())
			return nullptr;
		return &it->second;
	}


	ConfigSection& ConfigManager::GetSection(const std::string& domain, const std::string& section)
	{
		return GetDomain(domain).GetSection(section);
	}


	const ConfigSection* ConfigManager::GetSection(const std::string& domain, const std::string& section) const
	{
		const ConfigDomain* dom = GetDomain(domain);
		if (!dom)
			return nullptr;
		return dom->GetSection(section);
	}


	bool ConfigManager::HasDomain(const std::string& domain) const
	{
		std::shared_lock lock(m_mutex);
		return m_domains.find(domain) != m_domains.end();
	}


	template<typename T>
	T ConfigManager::Get(const std::string& domain, const std::string& section, const std::string& key, const T& defaultValue) const
	{
		const ConfigDomain* dom = GetDomain(domain);
		if (!dom)
			return defaultValue;
		return dom->Get<T>(section, key, defaultValue);
	}


	template<typename T>
	void ConfigManager::Set(const std::string& domain, const std::string& section, const std::string& key, const T& value)
	{
		GetDomain(domain).Set<T>(section, key, value);
	}


	// Explicit template instantiations
	template std::string ConfigManager::Get<std::string>(const std::string&, const std::string&, const std::string&, const std::string&) const;
	template bool ConfigManager::Get<bool>(const std::string&, const std::string&, const std::string&, const bool&) const;
	template int ConfigManager::Get<int>(const std::string&, const std::string&, const std::string&, const int&) const;
	template float ConfigManager::Get<float>(const std::string&, const std::string&, const std::string&, const float&) const;
	template double ConfigManager::Get<double>(const std::string&, const std::string&, const std::string&, const double&) const;

	template void ConfigManager::Set<std::string>(const std::string&, const std::string&, const std::string&, const std::string&);
	template void ConfigManager::Set<bool>(const std::string&, const std::string&, const std::string&, const bool&);
	template void ConfigManager::Set<int>(const std::string&, const std::string&, const std::string&, const int&);
	template void ConfigManager::Set<float>(const std::string&, const std::string&, const std::string&, const float&);
	template void ConfigManager::Set<double>(const std::string&, const std::string&, const std::string&, const double&);


	std::string ConfigManager::GetComment(const std::string& domain, const std::string& section, const std::string& key) const
	{
		const ConfigDomain* dom = GetDomain(domain);
		if (!dom)
			return "";
		return dom->GetComment(section, key);
	}


	std::string ConfigManager::GetDomainComment(const std::string& domain) const
	{
		const ConfigDomain* dom = GetDomain(domain);
		if (!dom)
			return "";
		return dom->GetDomainComment();
	}


	std::string ConfigManager::GetSectionComment(const std::string& domain, const std::string& section) const
	{
		const ConfigDomain* dom = GetDomain(domain);
		if (!dom)
			return "";
		return dom->GetSectionComment(section);
	}


	std::string ConfigManager::GetKeyComment(const std::string& domain, const std::string& section, const std::string& key) const
	{
		const ConfigDomain* dom = GetDomain(domain);
		if (!dom)
			return "";
		return dom->GetKeyComment(section, key);
	}


} // namespace Helios::Engine::Core
