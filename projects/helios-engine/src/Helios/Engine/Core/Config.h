//==============================================================================
// Configuration Management System
//
// Provides a thread-safe, hierarchical configuration system with three levels:
// ConfigManager (singleton) -> ConfigDomain (file-based) -> ConfigSection
// (key-value pairs). Supports loading/saving INI-style files, type-safe
// templated accessors, and per-level comment preservation. Uses shared_mutex
// for concurrent read access with exclusive write locking.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - Hierarchical config system (Manager->Domain->Section->Key)
// - Priority-based config layers (Default, Platform, Project, User, Runtime)
// - Automatic layer loading via VFS aliases (@config_default, @config_platform, etc.)
// - Thread-safe concurrent reads with shared_mutex
// - Type-safe templated Get/Set with defaults
// - Multi-level comment preservation
// - INI file persistence via VFS
// - Singleton pattern with auto-creation
// 
// Version history:
// - 2026.01: Added hierarchical priority-based configuration layers
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include "Helios/Engine/Util/IniParser.h"

//#include <string>
//#include <unordered_map>
//#include <shared_mutex>
//#include <vector>
//#include <algorithm>

namespace Helios::Engine {


	//------------------------------------------------------------------------------
	// Configuration Priority Levels
	//------------------------------------------------------------------------------

	enum class ConfigPriority : uint8_t {
		Default  = 0,  // Shipped with application    (@config_default),  read-only
		Platform = 1,  // Platform-specific overrides (@config_platform), read-only
		Project  = 2,  // Project/workspace settings  (@config_project),  read-only
		User     = 3,  // User account settings       (@config_user),     read/write
		Runtime  = 4   // Runtime overrides           (in-memory only),   read/write
	};


	//------------------------------------------------------------------------------
	// ConfigSection
	//------------------------------------------------------------------------------

	class ConfigDomain; // Forward declaration
	class ConfigSection
	{
	public:
		ConfigSection() = default;
		~ConfigSection() = default;

	public:
		bool HasKey(const std::string& key) const;

		// Templated getter/setter methods
		template<typename T>
		T Get(const std::string& key, const T& defaultValue = T{}) const;
		template<typename T>
		void Set(const std::string& key, const T& value);

		// Comment management
		std::string GetComment(const std::string& key = "") const
			{ return key.empty() ? GetSectionComment() : GetKeyComment(key); }

		std::string GetSectionComment() const
			{ std::shared_lock lock(m_mutex); return m_sectionComment; }
		std::string GetKeyComment(const std::string& key) const;

		void SetSectionComment(const std::string& comment)
			{ std::unique_lock lock(m_mutex); m_sectionComment = comment; }
		void SetKeyComment(const std::string& key, const std::string& comment)
			{ std::unique_lock lock(m_mutex); m_keyComments[key] = comment; }

		void ClearSectionComment()
			{ std::unique_lock lock(m_mutex); m_sectionComment.clear(); }
		void ClearKeyComment(const std::string& key)
			{ std::unique_lock lock(m_mutex); m_keyComments.erase(key); }

	private:
		// Helper
		template<typename T>
		static bool ParseValue(const std::string& str, T& out);
		template<typename T>
		static std::string StringifyValue(const T& value);
		template<typename T>
		inline static constexpr bool always_false = false;

	private:
		friend class ConfigDomain; // Allow ConfigDomain to access m_data
		mutable std::shared_mutex m_mutex;
		std::unordered_map<std::string, std::string> m_data;
		std::unordered_map<std::string, std::string> m_keyComments;
		std::string m_sectionComment;
	};


	//------------------------------------------------------------------------------
	// ConfigDomain
	//------------------------------------------------------------------------------

	
	class ConfigDomain
	{
	public:
		ConfigDomain() = delete;
		ConfigDomain(const std::string& filePath) : m_filePath(filePath) {};
		~ConfigDomain() = default;
		// Prevent copying
		ConfigDomain(const ConfigDomain&) = delete;
		ConfigDomain& operator=(const ConfigDomain&) = delete;
		// Allow moving
		ConfigDomain(ConfigDomain&&) = default;
		ConfigDomain& operator=(ConfigDomain&&) = default;

	public:
		bool Load(const std::string& filePath);
		bool Save(const std::string& filePath = "") const;
		void MergeInto(Util::IniParser& parser) const;
		std::string GetFilePath() const { std::shared_lock lock(m_mutex); return m_filePath; }

		ConfigSection& GetSection(const std::string& name);
		const ConfigSection* GetSection(const std::string& name) const;
		bool HasSection(const std::string& name) const;

		// Templated getter/setter methods
		template<typename T>
		T Get(const std::string& section, const std::string& key, const T& defaultValue = T{}) const;
		template<typename T>
		void Set(const std::string& section, const std::string& key, const T& value);

		// Comment management
		std::string GetComment(const std::string& section = "", const std::string& key = "") const;
		std::string GetDomainComment() const
			{ std::shared_lock lock(m_mutex); return m_domainComment; }
		std::string GetSectionComment(const std::string& section) const;
		std::string GetKeyComment(const std::string& section, const std::string& key) const;

		void SetDomainComment(const std::string& comment)
			{ std::unique_lock lock(m_mutex); m_domainComment = comment; }
		void SetSectionComment(const std::string& section, const std::string& comment);
		void SetKeyComment(const std::string& section, const std::string& key, const std::string& comment);

		void ClearDomainComment()
			{ std::unique_lock lock(m_mutex); m_domainComment.clear(); }
		void ClearSectionComment(const std::string& section);
		void ClearKeyComment(const std::string& section, const std::string& key);

	private:
		mutable std::shared_mutex m_mutex;
		std::string m_filePath;
		std::unordered_map<std::string, ConfigSection> m_sections;
		std::string m_domainComment;
	};


	//------------------------------------------------------------------------------
	// ConfigLayer - Represents a single priority layer in the hierarchy
	//------------------------------------------------------------------------------


	struct ConfigLayer {
		ConfigDomain domain;
		ConfigPriority priority;
		bool readOnly;
		bool loaded;

		ConfigLayer(const std::string& path, ConfigPriority prio, bool ro)
			: domain(path), priority(prio), readOnly(ro), loaded(false) {}

		// Explicitly default move operations (ConfigDomain supports move)
		ConfigLayer(ConfigLayer&&) = default;
		ConfigLayer& operator=(ConfigLayer&&) = default;

		// Delete copy operations (ConfigDomain doesn't support copy)
		ConfigLayer(const ConfigLayer&) = delete;
		ConfigLayer& operator=(const ConfigLayer&) = delete;
	};


	//------------------------------------------------------------------------------
	// ConfigManager - Singleton managing hierarchical configuration layers
	//------------------------------------------------------------------------------


	class ConfigManager
	{
	public:
		ConfigManager();
		~ConfigManager() = default;
		// Prevent copying
		ConfigManager(const ConfigManager&) = delete;
		ConfigManager& operator=(const ConfigManager&) = delete;

	public:
		static ConfigManager& GetInstance();

		// Hierarchical domain loading (auto-loads all priority layers via VFS aliases)
		// Expects VFS aliases: @config_default, @config_platform, @config_project, @config_user
		// Example: LoadDomain("engine") looks for:
		//   @config_default/engine.ini, @config_platform/engine.ini, etc.
		bool LoadDomain(const std::string& domain);

		// Save only the User layer (read-only layers are never modified)
		bool SaveDomain(const std::string& domain) const;

		// Get value with hierarchical fallback (searches User->Project->Platform->Default)
		template<typename T>
		T Get(const std::string& domain, const std::string& section, const std::string& key, const T& defaultValue = T{}) const;

		// Set value (writes only to User layer or Runtime if User is not loaded)
		template<typename T>
		void Set(const std::string& domain, const std::string& section, const std::string& key, const T& value);

		// Query which priority layer provides a value
		ConfigPriority GetValueSource(const std::string& domain, const std::string& section, const std::string& key) const;

		// Check if domain exists in any layer
		bool HasDomain(const std::string& domain) const;

		// Check if a specific layer is loaded for a domain
		bool HasLayer(const std::string& domain, ConfigPriority priority) const;

		// Comment management (operates on highest priority layer that has the key)
		std::string GetComment(const std::string& domain, const std::string& section = "", const std::string& key = "") const;
		std::string GetDomainComment(const std::string& domain) const;
		std::string GetSectionComment(const std::string& domain, const std::string& section) const;
		std::string GetKeyComment(const std::string& domain, const std::string& section, const std::string& key) const;

		void SetDomainComment(const std::string& domain, const std::string& comment);
		void SetSectionComment(const std::string& domain, const std::string& section, const std::string& comment);
		void SetKeyComment(const std::string& domain, const std::string& section, const std::string& key, const std::string& comment);

		void ClearDomainComment(const std::string& domain);
		void ClearSectionComment(const std::string& domain, const std::string& section);
		void ClearKeyComment(const std::string& domain, const std::string& section, const std::string& key);

	private:
		// Internal: Get layer by priority for a domain
		ConfigLayer* GetLayer(const std::string& domain, ConfigPriority priority);
		const ConfigLayer* GetLayer(const std::string& domain, ConfigPriority priority) const;

		// Internal: Get writable layer (User or Runtime)
		ConfigLayer* GetWritableLayer(const std::string& domain);

		// VFS alias mapping
		static const char* GetVFSAlias(ConfigPriority priority);

	private:
		mutable std::shared_mutex m_mutex;
		std::unordered_map<std::string, std::vector<std::unique_ptr<ConfigLayer>>> m_domainLayers;
	};


} // namespace Helios::Engine
