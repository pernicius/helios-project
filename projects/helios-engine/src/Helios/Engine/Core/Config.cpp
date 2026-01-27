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

#include "Helios/Engine/VFS/VFS.h"

#include <sstream>
#include <limits>
#include <charconv>

namespace Helios::Engine {


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


	void ConfigDomain::MergeInto(Util::IniParser& parser) const
	{
		std::shared_lock lock(m_mutex);

		// Set domain comment if the parser's one is empty
		if (parser.GetFileComment().empty()) {
			parser.SetFileComment(m_domainComment);
		}

		// Copy data and comments from sections to parser
		for (const auto& [sectionName, section] : m_sections) {
			// Set section comment if the parser's one is empty
			if (parser.GetSectionComment(sectionName).empty()) {
				parser.SetSectionComment(sectionName, section.GetSectionComment());
			}

			// Save data
			std::shared_lock secLock(section.m_mutex);
			for (const auto& [key, value] : section.m_data) {
				parser.Set<std::string>(sectionName, key, value);
			}

			// Save key comments
			for (const auto& [key, comment] : section.m_keyComments) {
				// Set key comment if the parser's one is empty
				if (parser.GetKeyComment(sectionName, key).empty()) {
					parser.SetKeyComment(sectionName, key, comment);
				}
			}
		}
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


	void ConfigDomain::SetSectionComment(const std::string& section, const std::string& comment)
	{
		ConfigSection& sec = GetSection(section);
		sec.SetSectionComment(comment);
	}


	void ConfigDomain::SetKeyComment(const std::string& section, const std::string& key, const std::string& comment)
	{
		ConfigSection& sec = GetSection(section);
		sec.SetKeyComment(key, comment);
	}


	void ConfigDomain::ClearSectionComment(const std::string& section)
	{
		ConfigSection& sec = GetSection(section);
		sec.ClearSectionComment();
	}


	void ConfigDomain::ClearKeyComment(const std::string& section, const std::string& key)
	{
		ConfigSection& sec = GetSection(section);
		sec.ClearKeyComment(key);
	}


	//------------------------------------------------------------------------------
	// ConfigManager Implementation
	//------------------------------------------------------------------------------


	ConfigManager::ConfigManager()
	{
		LOG_CORE_DEBUG("ConfigManager initialized.");
	}


	ConfigManager& ConfigManager::GetInstance()
	{
		static ConfigManager instance;
		return instance;
	}


	const char* ConfigManager::GetVFSAlias(ConfigPriority priority)
	{
		switch (priority) {
		case ConfigPriority::Default:  return "@config_default:";
		case ConfigPriority::Platform: return "@config_platform:";
		case ConfigPriority::Project:  return "@config_project:";
		case ConfigPriority::User:     return "@config_user:";
		case ConfigPriority::Runtime:  return ""; // Runtime is in-memory only
		default: return "";
		}
	}


	bool ConfigManager::LoadDomain(const std::string& domain)
	{
		std::unique_lock lock(m_mutex);

		auto& layers = m_domainLayers[domain];
		layers.clear();

		// Reserve space to avoid reallocations during emplace_back
		layers.reserve(5); // Default, Platform, Project, User, Runtime

		// Define priority order and read-only status
		struct LayerInfo {
			ConfigPriority priority;
			bool readOnly;
		};

		const LayerInfo layersToLoad[] = {
			{ ConfigPriority::Default,  true },
			{ ConfigPriority::Platform, true },
			{ ConfigPriority::Project,  true },
			{ ConfigPriority::User,     false },
		};

		bool anyLoaded = false;

		for (const auto& info : layersToLoad) {
			const char* alias = GetVFSAlias(info.priority);
			if (!alias || alias[0] == '\0')
				continue;

			// Construct VFS path: @config_default/engine.ini
			std::string vfsPath = std::string(alias) + "/" + domain + ".ini";

			// Check if file exists in VFS
			if (!VirtFS.Exists(vfsPath)) {
				LOG_CORE_TRACE("Config layer not found: {}", vfsPath);
				continue;
			}

			// Create layer
			layers.push_back(std::unique_ptr<ConfigLayer>(new ConfigLayer(vfsPath, info.priority, info.readOnly)));
			ConfigLayer& layer = *layers.back();

			// Try to load
			if (layer.domain.Load(vfsPath)) {
				layer.loaded = true;
				anyLoaded = true;
				LOG_CORE_INFO("Loaded config layer: {} (priority: {})", vfsPath, static_cast<int>(info.priority));
			}
			else {
				LOG_CORE_WARN("Failed to load config layer: {}", vfsPath);
			}
		}

		// Always create Runtime layer (in-memory only)
		layers.push_back(std::unique_ptr<ConfigLayer>(new ConfigLayer("", ConfigPriority::Runtime, false)));
		layers.back()->loaded = true; // Runtime is always "loaded"

		// No sorting needed - layers are already inserted in priority order (Default=0, Platform=1, Project=2, User=3, Runtime=4)

		return anyLoaded;
	}


	bool ConfigManager::SaveDomain(const std::string& domain) const
	{
		std::shared_lock lock(m_mutex);

		auto it = m_domainLayers.find(domain);
		if (it == m_domainLayers.end()) {
			LOG_CORE_WARN("Cannot save domain '{}': Domain not loaded.", domain);
			return false;
		}

		// Find the highest-priority writable layer (User or Runtime) that contains data.
		// We need to merge data from Runtime down into User for persistence.
		const ConfigLayer* runtimeLayer = GetLayer(domain, ConfigPriority::Runtime);
		const ConfigLayer* userLayer = GetLayer(domain, ConfigPriority::User);

		// If neither layer exists, there's nothing to save.
		if (!runtimeLayer && !userLayer) {
			LOG_CORE_WARN("Cannot save domain '{}': No writable layers (User, Runtime) found.", domain);
			return false;
		}

		// Construct the save path for the User layer. This is where we always save.
		const char* alias = GetVFSAlias(ConfigPriority::User);
		if (!alias || alias[0] == '\0') {
			LOG_CORE_ERROR("Cannot save domain '{}': VFS alias for User priority is not defined.", domain);
			return false;
		}
		std::string savePath = std::string(alias) + "/" + domain + ".ini";

		// Create a temporary parser to merge data for saving.
		Helios::Util::IniParser mergedParser;

		// 1. Load the existing user config file into the parser, if it exists.
		// This preserves any values and comments not touched during runtime.
		if (userLayer && userLayer->loaded) {
			mergedParser.Load(userLayer->domain.GetFilePath());
		}

		// 2. Merge data from the in-memory User layer (if it was loaded).
		if (userLayer) {
			userLayer->domain.MergeInto(mergedParser);
		}

		// 3. Merge data from the Runtime layer, overwriting any existing values.
		if (runtimeLayer) {
			runtimeLayer->domain.MergeInto(mergedParser);
		}

		lock.unlock();
		bool result = mergedParser.Save(savePath);

		if (result) {
			LOG_CORE_INFO("Saved user config for domain '{}' to: {}", domain, savePath);
		}
		else {
			LOG_CORE_ERROR("Failed to save user config for domain '{}' to: {}", domain, savePath);
		}

		return result;
	}


	ConfigLayer* ConfigManager::GetLayer(const std::string& domain, ConfigPriority priority)
	{
		auto it = m_domainLayers.find(domain);
		if (it == m_domainLayers.end())
			return nullptr;

		auto layerIt = std::find_if(it->second.begin(), it->second.end(),
			[priority](const std::unique_ptr<ConfigLayer>& layer) {
				return layer->priority == priority && layer->loaded;
			});

		return layerIt != it->second.end() ? layerIt->get() : nullptr;
	}


	const ConfigLayer* ConfigManager::GetLayer(const std::string& domain, ConfigPriority priority) const
	{
		auto it = m_domainLayers.find(domain);
		if (it == m_domainLayers.end())
			return nullptr;

		auto layerIt = std::find_if(it->second.begin(), it->second.end(),
			[priority](const std::unique_ptr<ConfigLayer>& layer) {
				return layer->priority == priority && layer->loaded;
			});

		return layerIt != it->second.end() ? layerIt->get() : nullptr;
	}


	ConfigLayer* ConfigManager::GetWritableLayer(const std::string& domain)
	{
		auto it = m_domainLayers.find(domain);
		if (it == m_domainLayers.end())
			return nullptr;

		// Try User first, then Runtime
		for (auto priority : { ConfigPriority::User, ConfigPriority::Runtime }) {
			auto layerIt = std::find_if(it->second.begin(), it->second.end(),
				[priority](const std::unique_ptr<ConfigLayer>& layer) {
					return layer->priority == priority && !layer->readOnly && layer->loaded;
				});

			if (layerIt != it->second.end())
				return layerIt->get();
		}

		return nullptr;
	}


	template<typename T>
	T ConfigManager::Get(const std::string& domain, const std::string& section, const std::string& key, const T& defaultValue) const
	{
		std::shared_lock lock(m_mutex);

		auto it = m_domainLayers.find(domain);
		if (it == m_domainLayers.end())
			return defaultValue;

		// Search from highest priority (Runtime) to lowest (Default)
		for (auto layerIt = it->second.rbegin(); layerIt != it->second.rend(); ++layerIt) {
			if (!(*layerIt)->loaded)
				continue;

			const ConfigSection* sec = static_cast<const ConfigDomain&>((*layerIt)->domain).GetSection(section);
			if (sec && sec->HasKey(key)) {
				return sec->Get<T>(key, defaultValue);
			}
		}

		return defaultValue;
	}


	template<typename T>
	void ConfigManager::Set(const std::string& domain, const std::string& section, const std::string& key, const T& value)
	{
		std::unique_lock lock(m_mutex);

		ConfigLayer* layer = GetWritableLayer(domain);
		if (!layer) {
			// Auto-create Runtime layer if no writable layer exists
			auto& layers = m_domainLayers[domain];
			layers.push_back(std::unique_ptr<ConfigLayer>(new ConfigLayer("", ConfigPriority::Runtime, false)));
			layers.back()->loaded = true;

			// No sorting needed - Runtime has highest priority and is added last

			layer = GetWritableLayer(domain);
		}

		lock.unlock();
		layer->domain.Set<T>(section, key, value);
	}


	ConfigPriority ConfigManager::GetValueSource(const std::string& domain, const std::string& section, const std::string& key) const
	{
		std::shared_lock lock(m_mutex);

		auto it = m_domainLayers.find(domain);
		if (it == m_domainLayers.end())
			return ConfigPriority::Default;

		// Search from highest priority to lowest
		for (auto layerIt = it->second.rbegin(); layerIt != it->second.rend(); ++layerIt) {
			if (!(*layerIt)->loaded)
				continue;

			const ConfigSection* sec = static_cast<const ConfigDomain&>((*layerIt)->domain).GetSection(section);
			if (sec && sec->HasKey(key)) {
				return (*layerIt)->priority;
			}
		}

		return ConfigPriority::Default;
	}


	bool ConfigManager::HasDomain(const std::string& domain) const
	{
		std::shared_lock lock(m_mutex);
		return m_domainLayers.find(domain) != m_domainLayers.end();
	}


	bool ConfigManager::HasLayer(const std::string& domain, ConfigPriority priority) const
	{
		std::shared_lock lock(m_mutex);
		return GetLayer(domain, priority) != nullptr;
	}


	std::string ConfigManager::GetComment(const std::string& domain, const std::string& section, const std::string& key) const
	{
		if (section.empty())
			return GetDomainComment(domain);
		if (key.empty())
			return GetSectionComment(domain, section);
		return GetKeyComment(domain, section, key);
	}


	std::string ConfigManager::GetDomainComment(const std::string& domain) const
	{
		std::shared_lock lock(m_mutex);

		auto it = m_domainLayers.find(domain);
		if (it == m_domainLayers.end())
			return "";

		// Return comment from highest priority layer that has one
		for (auto layerIt = it->second.rbegin(); layerIt != it->second.rend(); ++layerIt) {
			if (!(*layerIt)->loaded)
				continue;
			std::string comment = (*layerIt)->domain.GetDomainComment();
			if (!comment.empty())
				return comment;
		}

		return "";
	}


	std::string ConfigManager::GetSectionComment(const std::string& domain, const std::string& section) const
	{
		std::shared_lock lock(m_mutex);

		auto it = m_domainLayers.find(domain);
		if (it == m_domainLayers.end())
			return "";

		for (auto layerIt = it->second.rbegin(); layerIt != it->second.rend(); ++layerIt) {
			if (!(*layerIt)->loaded)
				continue;
			const ConfigSection* sec = static_cast<const ConfigDomain&>((*layerIt)->domain).GetSection(section);
			if (sec) {
				std::string comment = sec->GetSectionComment();
				if (!comment.empty())
					return comment;
			}
		}

		return "";
	}


	std::string ConfigManager::GetKeyComment(const std::string& domain, const std::string& section, const std::string& key) const
	{
		std::shared_lock lock(m_mutex);

		auto it = m_domainLayers.find(domain);
		if (it == m_domainLayers.end())
			return "";

		for (auto layerIt = it->second.rbegin(); layerIt != it->second.rend(); ++layerIt) {
			if (!(*layerIt)->loaded)
				continue;
			const ConfigSection* sec = static_cast<const ConfigDomain&>((*layerIt)->domain).GetSection(section);
			if (sec && sec->HasKey(key)) {
				std::string comment = sec->GetKeyComment(key);
				if (!comment.empty())
					return comment;
			}
		}

		return "";
	}


	void ConfigManager::SetDomainComment(const std::string& domain, const std::string& comment)
	{
		std::unique_lock lock(m_mutex);
		ConfigLayer* layer = GetWritableLayer(domain);
		if (layer) {
			lock.unlock();
			layer->domain.SetDomainComment(comment);
		}
	}


	void ConfigManager::SetSectionComment(const std::string& domain, const std::string& section, const std::string& comment)
	{
		std::unique_lock lock(m_mutex);
		ConfigLayer* layer = GetWritableLayer(domain);
		if (layer) {
			lock.unlock();
			layer->domain.SetSectionComment(section, comment);
		}
	}


	void ConfigManager::SetKeyComment(const std::string& domain, const std::string& section, const std::string& key, const std::string& comment)
	{
		std::unique_lock lock(m_mutex);
		ConfigLayer* layer = GetWritableLayer(domain);
		if (layer) {
			lock.unlock();
			layer->domain.SetKeyComment(section, key, comment);
		}
	}


	void ConfigManager::ClearDomainComment(const std::string& domain)
	{
		std::unique_lock lock(m_mutex);
		ConfigLayer* layer = GetWritableLayer(domain);
		if (layer) {
			lock.unlock();
			layer->domain.ClearDomainComment();
		}
	}


	void ConfigManager::ClearSectionComment(const std::string& domain, const std::string& section)
	{
		std::unique_lock lock(m_mutex);
		ConfigLayer* layer = GetWritableLayer(domain);
		if (layer) {
			lock.unlock();
			layer->domain.ClearSectionComment(section);
		}
	}


	void ConfigManager::ClearKeyComment(const std::string& domain, const std::string& section, const std::string& key)
	{
		std::unique_lock lock(m_mutex);
		ConfigLayer* layer = GetWritableLayer(domain);
		if (layer) {
			lock.unlock();
			layer->domain.ClearKeyComment(section, key);
		}
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


} // namespace Helios::Engine
