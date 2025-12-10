#pragma once

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cerrno>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace Helios::Util {


	class IniParser
	{
	public:
		using Section = std::unordered_map<std::string, std::string>;

		IniParser() = default;


		bool load(const std::filesystem::path& path)
		{
			data_.clear();
			std::ifstream ifs(path, std::ios::binary);
			if (!ifs)
				return false;

			std::string line;
			std::string currentSection;
			while (std::getline(ifs, line))
			{
				// remove BOM on first line if present
				if (!line.empty() && static_cast<unsigned char>(line[0]) == 0xEF)
				{
					if (line.size() >= 3 &&
						static_cast<unsigned char>(line[0]) == 0xEF &&
						static_cast<unsigned char>(line[1]) == 0xBB &&
						static_cast<unsigned char>(line[2]) == 0xBF)
					{
						line.erase(0, 3);
					}
				}

				trim_inplace(line);
				if (line.empty())
					continue;

				// comments
				if (line.front() == ';' || line.front() == '#')
					continue;

				// section header
				if (line.front() == '[' && line.back() == ']')
				{
					currentSection = line.substr(1, line.size() - 2);
					trim_inplace(currentSection);
					continue;
				}

				// key/value: support "key = value" and "key: value"
				auto posEq = line.find('=');
				auto posCol = line.find(':');
				std::size_t sep = std::string::npos;
				if (posEq != std::string::npos)
					sep = posEq;
				else if (posCol != std::string::npos)
					sep = posCol;

				if (sep == std::string::npos)
					continue; // ignore malformed lines

				std::string key = line.substr(0, sep);
				std::string value = line.substr(sep + 1);
				trim_inplace(key);
				trim_inplace(value);

				data_[currentSection][key] = value;
			}

			return true;
		}


		bool save(const std::filesystem::path& path) const
		{
			std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
			if (!ofs)
				return false;

			for (const auto& [section, kv] : data_)
			{
				if (!section.empty())
					ofs << "[" << section << "]\n";
				for (const auto& [k, v] : kv)
					ofs << k << " = " << v << "\n";
				ofs << "\n";
			}
			return true;
		}


		// Template-based getter: strong typed conversion with default
		template<typename T>
		T get(const std::string& section, const std::string& key, const T& def) const
		{
			auto sit = data_.find(section);
			if (sit == data_.end())
				return def;
			auto kit = sit->second.find(key);
			if (kit == sit->second.end())
				return def;

			const std::string& s = kit->second;
			if (s.empty())
				return def;

			T out{};
			if (from_string(s, out))
				return out;
			return def;
		}


		// Template-based setter: converts value to string using type-aware formatting
		template<typename T>
		void set(const std::string& section, const std::string& key, const T& value)
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				data_[section][key] = value;
			}
			else if constexpr (std::is_same_v<T, const char*>)
			{
				data_[section][key] = std::string(value);
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				data_[section][key] = value ? "true" : "false";
			}
			else if constexpr (std::is_integral_v<T>)
			{
				data_[section][key] = std::to_string(value);
			}
			else if constexpr (std::is_floating_point_v<T>)
			{
				std::ostringstream ss;
				ss.precision(std::numeric_limits<T>::digits10 + 1);
				ss << value;
				data_[section][key] = ss.str();
			}
			else
			{
				static_assert(always_false<T>, "IniParser::set<T> unsupported type - provide a string or numeric/bool type");
			}
		}


		bool has(const std::string& section, const std::string& key) const
		{
			auto sit = data_.find(section);
			if (sit == data_.end())
				return false;
			return sit->second.find(key) != sit->second.end();
		}


		std::vector<std::string> sections() const
		{
			std::vector<std::string> out;
			out.reserve(data_.size());
			for (const auto& p : data_)
				out.push_back(p.first);
			return out;
		}


		std::vector<std::string> keys(const std::string& section) const
		{
			std::vector<std::string> out;
			auto sit = data_.find(section);
			if (sit == data_.end())
				return out;
			out.reserve(sit->second.size());
			for (const auto& kv : sit->second)
				out.push_back(kv.first);
			return out;
		}

	private:
		std::unordered_map<std::string, Section> data_;

		template<typename T>
		inline static constexpr bool always_false = false;


		static inline void trim_inplace(std::string& s)
		{
			auto not_space = [](unsigned char ch) { return !std::isspace(ch); };
			// left
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
			// right
			s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
		}


		static inline std::string to_lower_copy(const std::string& s)
		{
			std::string out = s;
			std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return out;
		}


		// Generic from_string supporting integral, floating, and bool types
		template<typename T>
		static bool from_string(const std::string& s, T& out)
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				out = s;
				return true;
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				return parse_bool(s, out);
			}
			else if constexpr (std::is_integral_v<T>)
			{
				if (s.empty())
					return false;
				const char* begin = s.c_str();
				const char* end = begin + s.size();
				std::from_chars_result res = std::from_chars(begin, end, out);
				return res.ec == std::errc() && res.ptr == end;
			}
			else if constexpr (std::is_floating_point_v<T>)
			{
				double tmp = 0.0;
				if (!parse_double(s, tmp))
					return false;
				out = static_cast<T>(tmp);
				return true;
			}
			else
			{
				// unsupported type
				return false;
			}
		}


		// Floating parsing using strtod for wide compatibility, validating consumption
		static inline bool parse_double(const std::string& s, double& out)
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
			while (*endptr != '\0')
			{
				if (!std::isspace(static_cast<unsigned char>(*endptr)))
					return false;
				++endptr;
			}
			return true;
		}


		// Boolean parsing supporting: 1/0, true/false, yes/no, on/off (case-insensitive)
		static inline bool parse_bool(const std::string& s, bool& out)
		{
			if (s.empty())
				return false;
			std::string lower = to_lower_copy(s);
			if (lower == "1" || lower == "true" || lower == "yes" || lower == "on" || lower == "y")
			{
				out = true;
				return true;
			}
			if (lower == "0" || lower == "false" || lower == "no" || lower == "off" || lower == "n")
			{
				out = false;
				return true;
			}
			// numeric fallback: treat any non-zero number as true
			long long n = 0;
			if (parse_int64(lower, n))
			{
				out = (n != 0);
				return true;
			}
			return false;
		}


		// Integer parsing helper for fallback numeric boolean parsing
		static inline bool parse_int64(const std::string& s, long long& out)
		{
			if (s.empty())
				return false;
			const char* begin = s.c_str();
			const char* end = begin + s.size();
			std::from_chars_result res = std::from_chars(begin, end, out);
			return res.ec == std::errc() && res.ptr == end;
		}
	};


} // namespace Helios::Util
