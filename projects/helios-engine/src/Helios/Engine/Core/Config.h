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
// - Thread-safe concurrent reads with shared_mutex
// - Type-safe templated Get/Set with defaults
// - Multi-level comment preservation
// - INI file persistence via VFS
// - Singleton pattern with auto-creation
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

//#include <string>
//#include <unordered_map>
//#include <shared_mutex>

namespace Helios::Engine {


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
		void SetSectionComment(const std::string& section, const std::string& comment)
			{ GetSection(section).SetSectionComment(comment); }
		void SetKeyComment(const std::string& section, const std::string& key, const std::string& comment)
			{ GetSection(section).SetKeyComment(key, comment); }

		void ClearDomainComment()
			{ std::unique_lock lock(m_mutex); m_domainComment.clear(); }
		void ClearSectionComment(const std::string& section)
			{ GetSection(section).ClearSectionComment(); }
		void ClearKeyComment(const std::string& section, const std::string& key)
			{ GetSection(section).ClearKeyComment(key); }

	private:
		mutable std::shared_mutex m_mutex;
		std::string m_filePath;
		std::unordered_map<std::string, ConfigSection> m_sections;
		std::string m_domainComment;
	};


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

		bool LoadDomain(const std::string& domain, const std::string& filePath);
		bool SaveDomain(const std::string& domain, const std::string& filePath = "") const;

		ConfigDomain& GetDomain(const std::string& domain);
		const ConfigDomain* GetDomain(const std::string& domain) const;
		ConfigSection& GetSection(const std::string& domain, const std::string& section);
		const ConfigSection* GetSection(const std::string& domain, const std::string& section) const;
		bool HasDomain(const std::string& domain) const;

		// Templated getter/setter methods
		template<typename T>
		T Get(const std::string& domain, const std::string& section, const std::string& key, const T& defaultValue = T{}) const;
		template<typename T>
		void Set(const std::string& domain, const std::string& section, const std::string& key, const T& value);

		// Comment management
		std::string GetComment(const std::string& domain, const std::string& section = "", const std::string& key = "") const;
		std::string GetDomainComment(const std::string& domain) const;
		std::string GetSectionComment(const std::string& domain, const std::string& section) const;
		std::string GetKeyComment(const std::string& domain, const std::string& section, const std::string& key) const;
		
		void SetDomainComment(const std::string& domain, const std::string& comment)
			{ GetDomain(domain).SetDomainComment(comment); }
		void SetSectionComment(const std::string& domain, const std::string& section, const std::string& comment)
			{ GetDomain(domain).SetSectionComment(section, comment); }
		void SetKeyComment(const std::string& domain, const std::string& section, const std::string& key, const std::string& comment)
			{ GetDomain(domain).SetKeyComment(section, key, comment); }

		void ClearDomainComment(const std::string& domain)
			{ GetDomain(domain).ClearDomainComment(); }
		void ClearSectionComment(const std::string& domain, const std::string& section)
			{ GetDomain(domain).ClearSectionComment(section); }
		void ClearKeyComment(const std::string& domain, const std::string& section, const std::string& key)
			{ GetDomain(domain).ClearKeyComment(section, key); }

	private:
		mutable std::shared_mutex m_mutex;
		std::unordered_map<std::string, ConfigDomain> m_domains;
	};


} // namespace Helios::Engine::Core
