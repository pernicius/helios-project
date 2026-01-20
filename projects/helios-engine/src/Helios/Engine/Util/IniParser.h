#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace Helios::Util {


	class IniParser
	{
	public:
		using Section = std::unordered_map<std::string, std::string>;

		IniParser() = default;

	public:
		bool Load(const std::filesystem::path& path);
		bool Save(const std::filesystem::path& path) const;

		// Comment management
		void SetFileComment(const std::string& comment);
		std::string GetFileComment() const;
		void SetSectionComment(const std::string& section, const std::string& comment);
		std::string GetSectionComment(const std::string& section) const;
		void SetKeyComment(const std::string& section, const std::string& key, const std::string& comment);
		std::string GetKeyComment(const std::string& section, const std::string& key) const;

		// Template-based getter: strong typed conversion with default.
		// If the requested key/section is missing or invalid, the default value
		// is stored in-memory so a subsequent Save() will persist it.
		template<typename T>
		T Get(const std::string& section, const std::string& key, const T& def, bool dontStore = false) const;

		// Template-based setter: converts value to string using type-aware formatting
		template<typename T>
		void Set(const std::string& section, const std::string& key, const T& value);

		bool Has(const std::string& section, const std::string& key) const;
		std::vector<std::string> Sections() const;
		std::vector<std::string> Keys(const std::string& section) const;
		void ClearSection(const std::string& section);

	private:
		mutable std::unordered_map<std::string, Section> m_data;
		mutable std::unordered_map<std::string, std::string> m_keyComments;     // "section.key" -> comment
		mutable std::unordered_map<std::string, std::string> m_sectionComments; // "section" -> comment
		std::string m_fileComment;  // File header comment

		template<typename T>
		inline static constexpr bool always_false = false;

		static void TrimInPlace(std::string& s);
		static std::string ToLowerCopy(const std::string& s);
		static std::string JoinComments(const std::vector<std::string>& comments);
		static void WriteComment(std::ofstream& ofs, const std::string& comment);

		template<typename T>
		static bool ParseValue(const std::string& s, T& out);

		template<typename T>
		static std::string StringifyValue(const T& value);

		static bool ParseDouble(const std::string& s, double& out);
		static bool ParseBool(const std::string& s, bool& out);
		static bool ParseInt64(const std::string& s, long long& out);
	};


} // namespace Helios::Util
