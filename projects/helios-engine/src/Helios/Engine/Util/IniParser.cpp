//==============================================================================
// INI File Parser (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file IniParser.h
//==============================================================================
#include "pch.h"
#include "Helios/Engine/Util/IniParser.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <sstream>
#include <system_error>
#include <type_traits>

namespace Helios::Util {


	bool IniParser::Load(const std::filesystem::path& path)
	{
		m_data.clear();
		m_keyComments.clear();
		m_sectionComments.clear();
		m_fileComment.clear();

		std::ifstream ifs(path, std::ios::binary);
		if (!ifs)
			return false;

		std::string line;
		std::string currentSection;
		std::vector<std::string> pendingComments;
		bool isFileHeader = true;

		while (std::getline(ifs, line)) {
			// Remove BOM on first line if present
			if (!line.empty() && static_cast<unsigned char>(line[0]) == 0xEF) {
				if (line.size() >= 3 &&
					static_cast<unsigned char>(line[0]) == 0xEF &&
					static_cast<unsigned char>(line[1]) == 0xBB &&
					static_cast<unsigned char>(line[2]) == 0xBF) {
					line.erase(0, 3);
				}
			}

			std::string trimmedLine = line;
			TrimInPlace(trimmedLine);

			// Empty line handling
			if (trimmedLine.empty()) {
				// Empty lines can separate file header from content
				if (isFileHeader && !pendingComments.empty()) {
					m_fileComment = JoinComments(pendingComments);
					pendingComments.clear();
				}
				continue;
			}

			// Comment line
			if (trimmedLine.front() == ';' || trimmedLine.front() == '#') {
				pendingComments.push_back(trimmedLine);
				continue;
			}

			// Section header
			if (trimmedLine.front() == '[' && trimmedLine.back() == ']') {
				isFileHeader = false;
				currentSection = trimmedLine.substr(1, trimmedLine.size() - 2);
				TrimInPlace(currentSection);

				// Attach pending comments to section
				if (!pendingComments.empty()) {
					m_sectionComments[currentSection] = JoinComments(pendingComments);
					pendingComments.clear();
				}
				continue;
			}

			// key/value: support "key = value" and "key: value"
			auto posEq = trimmedLine.find('=');
			auto posCol = trimmedLine.find(':');
			std::size_t sep = std::string::npos;
			if (posEq != std::string::npos)
				sep = posEq;
			else if (posCol != std::string::npos)
				sep = posCol;

			if (sep == std::string::npos) {
				// Malformed line - clear pending comments
				pendingComments.clear();
				continue;
			}

			isFileHeader = false;

			std::string key = trimmedLine.substr(0, sep);
			std::string value = trimmedLine.substr(sep + 1);
			TrimInPlace(key);
			TrimInPlace(value);

			m_data[currentSection][key] = value;

			// Attach pending comments to key
			if (!pendingComments.empty()) {
				std::string fullKey = currentSection + "." + key;
				m_keyComments[fullKey] = JoinComments(pendingComments);
				pendingComments.clear();
			}
		}

		// Remaining comments become file header if no content was parsed
		if (isFileHeader && !pendingComments.empty()) {
			m_fileComment = JoinComments(pendingComments);
		}

		return true;
	}


	bool IniParser::Save(const std::filesystem::path& path) const
	{
		// Ensure parent directory exists
		std::error_code ec;
		auto dir = path.parent_path();
		if (!dir.empty()) {
			std::filesystem::create_directories(dir, ec);
			if (ec)
				return false;
		}

		std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
		if (!ofs)
			return false;

		// Write file header comment
		if (!m_fileComment.empty()) {
			WriteComment(ofs, m_fileComment);
			ofs << "\n";
		}

		for (const auto& [section, kv] : m_data) {
			if (section.empty() && kv.empty())
				continue;

			// Write section comment
			if (!section.empty()) {
				auto scIt = m_sectionComments.find(section);
				if (scIt != m_sectionComments.end() && !scIt->second.empty()) {
					WriteComment(ofs, scIt->second);
				}
				ofs << "[" << section << "]\n";
			}

			// Write key-value pairs with their comments
			for (const auto& [k, v] : kv) {
				std::string fullKey = section + "." + k;
				auto kcIt = m_keyComments.find(fullKey);
				if (kcIt != m_keyComments.end() && !kcIt->second.empty()) {
					WriteComment(ofs, kcIt->second);
				}
				ofs << k << " = " << v << "\n";
			}
			ofs << "\n";
		}
		return true;
	}


	void IniParser::SetFileComment(const std::string& comment)
	{
		m_fileComment = comment;
	}


	std::string IniParser::GetFileComment() const
	{
		return m_fileComment;
	}


	void IniParser::SetSectionComment(const std::string& section, const std::string& comment)
	{
		m_sectionComments[section] = comment;
	}


	std::string IniParser::GetSectionComment(const std::string& section) const
	{
		auto it = m_sectionComments.find(section);
		return (it != m_sectionComments.end()) ? it->second : std::string{};
	}


	void IniParser::SetKeyComment(const std::string& section, const std::string& key, const std::string& comment)
	{
		std::string fullKey = section + "." + key;
		m_keyComments[fullKey] = comment;
	}


	std::string IniParser::GetKeyComment(const std::string& section, const std::string& key) const
	{
		std::string fullKey = section + "." + key;
		auto it = m_keyComments.find(fullKey);
		return (it != m_keyComments.end()) ? it->second : std::string{};
	}


	bool IniParser::Has(const std::string& section, const std::string& key) const
	{
		auto sit = m_data.find(section);
		if (sit == m_data.end())
			return false;
		return sit->second.find(key) != sit->second.end();
	}


	std::vector<std::string> IniParser::Sections() const
	{
		std::vector<std::string> out;
		out.reserve(m_data.size());
		for (const auto& p : m_data)
			out.push_back(p.first);
		return out;
	}


	std::vector<std::string> IniParser::Keys(const std::string& section) const
	{
		std::vector<std::string> out;
		auto sit = m_data.find(section);
		if (sit == m_data.end())
			return out;
		out.reserve(sit->second.size());
		for (const auto& kv : sit->second)
			out.push_back(kv.first);
		return out;
	}


	void IniParser::ClearSection(const std::string& section)
	{
		m_data.erase(section);
		m_sectionComments.erase(section);

		// Clear key comments for this section
		std::string prefix = section + ".";
		for (auto it = m_keyComments.begin(); it != m_keyComments.end();) {
			if (it->first.rfind(prefix, 0) == 0)
				it = m_keyComments.erase(it);
			else
				++it;
		}
	}


	//------------------------------------------------------------------------------
	// Helper Methods
	//------------------------------------------------------------------------------


	void IniParser::TrimInPlace(std::string& s)
	{
		auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };
		// left
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
		// right
		s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
	}


	std::string IniParser::ToLowerCopy(const std::string& s)
	{
		std::string out = s;
		std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return out;
	}


	std::string IniParser::JoinComments(const std::vector<std::string>& comments)
	{
		std::string result;
		for (size_t i = 0; i < comments.size(); ++i) {
			result += comments[i];
			if (i < comments.size() - 1)
				result += "\n";
		}
		return result;
	}


	void IniParser::WriteComment(std::ofstream& ofs, const std::string& comment)
	{
		std::istringstream iss(comment);
		std::string line;
		while (std::getline(iss, line)) {
			ofs << line << "\n";
		}
	}


	bool IniParser::ParseDouble(const std::string& s, double& out)
	{
		if (s.empty())
			return false;
		char* endptr = nullptr;
		errno = 0;
		const char* cstr = s.c_str();
		out = std::strtod(cstr, &endptr);
		if (endptr == cstr)
			return false; // no conversion
		if (errno == ERANGE)
			return false; // out of range
		while (*endptr != '\0') {
			if (!std::isspace(static_cast<unsigned char>(*endptr)))
				return false;
			++endptr;
		}
		return true;
	}


	bool IniParser::ParseBool(const std::string& s, bool& out)
	{
		if (s.empty())
			return false;
		std::string lower = ToLowerCopy(s);
		if (lower == "1" || lower == "true" || lower == "yes" || lower == "on" || lower == "y") {
			out = true;
			return true;
		}
		if (lower == "0" || lower == "false" || lower == "no" || lower == "off" || lower == "n") {
			out = false;
			return true;
		}
		// numeric fallback: treat any non-zero number as true
		long long n = 0;
		if (ParseInt64(lower, n)) {
			out = (n != 0);
			return true;
		}
		return false;
	}


	bool IniParser::ParseInt64(const std::string& s, long long& out)
	{
		if (s.empty())
			return false;
		const char* begin = s.c_str();
		const char* end = begin + s.size();
		std::from_chars_result res = std::from_chars(begin, end, out);
		return res.ec == std::errc() && res.ptr == end;
	}


	//------------------------------------------------------------------------------
	// Template Implementations
	//------------------------------------------------------------------------------


	template<typename T>
	T IniParser::Get(const std::string& section, const std::string& key, const T& def, bool dontStore) const
	{
		auto sit = m_data.find(section);
		if (sit == m_data.end()) {
			if (dontStore)
				return def;
			// create section and store default
			m_data[section][key] = StringifyValue(def);
			return def;
		}
		auto kit = sit->second.find(key);
		if (kit == sit->second.end()) {
			if (dontStore)
				return def;
			// store default for missing key
			m_data[section][key] = StringifyValue(def);
			return def;
		}

		const std::string& s = kit->second;
		if (s.empty()) {
			if (dontStore)
				return def;
			// empty value -> replace with default
			m_data[section][key] = StringifyValue(def);
			return def;
		}

		T out{};
		if (ParseValue(s, out))
			return out;

		// parse failed...
		if (dontStore)
			return def;
		// overwrite with default so Save() persists a valid value
		m_data[section][key] = StringifyValue(def);
		return def;
	}


	template<typename T>
	void IniParser::Set(const std::string& section, const std::string& key, const T& value)
	{
		m_data[section][key] = StringifyValue(value);
	}


	template<typename T>
	bool IniParser::ParseValue(const std::string& s, T& out)
	{
		if constexpr (std::is_same_v<T, std::string>) {
			out = s;
			return true;
		}
		else if constexpr (std::is_same_v<T, bool>) {
			return ParseBool(s, out);
		}
		else if constexpr (std::is_integral_v<T>) {
			if (s.empty())
				return false;
			const char* begin = s.c_str();
			const char* end = begin + s.size();
			std::from_chars_result res = std::from_chars(begin, end, out);
			return res.ec == std::errc() && res.ptr == end;
		}
		else if constexpr (std::is_floating_point_v<T>) {
			double tmp = 0.0;
			if (!ParseDouble(s, tmp))
				return false;
			out = static_cast<T>(tmp);
			return true;
		}
		else {
			// unsupported type
			return false;
		}
	}


	template<typename T>
	std::string IniParser::StringifyValue(const T& value)
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
			static_assert(always_false<T>, "IniParser::StringifyValue unsupported type - provide a string or numeric/bool type");
			return std::string{};
		}
	}


	// Explicit template instantiations for common types
	template std::string IniParser::Get<std::string>(const std::string&, const std::string&, const std::string&, bool) const;
	template bool IniParser::Get<bool>(const std::string&, const std::string&, const bool&, bool) const;
	template int IniParser::Get<int>(const std::string&, const std::string&, const int&, bool) const;
	template float IniParser::Get<float>(const std::string&, const std::string&, const float&, bool) const;
	template double IniParser::Get<double>(const std::string&, const std::string&, const double&, bool) const;

	template void IniParser::Set<std::string>(const std::string&, const std::string&, const std::string&);
	template void IniParser::Set<bool>(const std::string&, const std::string&, const bool&);
	template void IniParser::Set<int>(const std::string&, const std::string&, const int&);
	template void IniParser::Set<float>(const std::string&, const std::string&, const float&);
	template void IniParser::Set<double>(const std::string&, const std::string&, const double&);


} // namespace Helios::Util
