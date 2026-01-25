//==============================================================================
// Application Specification (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file AppSpec.h
//==============================================================================
#include "pch.h"
#include "Helios/Engine/Core/AppSpec.h"

namespace Helios::Engine {


	namespace { // internal helpers for CommandLineArgs

		inline bool ci_equal(std::string_view a, std::string_view b) noexcept
		{
			if (a.size() != b.size()) return false;
			for (size_t i = 0; i < a.size(); ++i)
			{
				if (std::tolower(static_cast<unsigned char>(a[i])) !=
					std::tolower(static_cast<unsigned char>(b[i])))
					return false;
			}
			return true;
		}

		inline void split_arg(std::string_view raw, std::string_view& out_key, std::string_view& out_val) noexcept
		{
			out_key = {};
			out_val = {};

			size_t start = raw.find_first_not_of("/-");
			if (start == std::string_view::npos)
				return;

			raw.remove_prefix(start);
			size_t eq_pos = raw.find('=');
			out_key = raw.substr(0, eq_pos);
			if (eq_pos != std::string_view::npos)
				out_val = raw.substr(eq_pos + 1);
		}

	} // internal helpers for CommandLineArgs


	// ----------------------------------------------------------------------------------------------------


	bool AppSpec::CommandLineArgs::Check(std::string_view arg) const
	{
		if (arg.empty())
			return false;

		for (int x = 1; x < Count; ++x)
		{
			std::string_view raw_arg(Args[x]);
			std::string_view key, value;
			split_arg(raw_arg, key, value);

			if (key.empty())
				continue;

			if (ci_equal(key, arg))
				return true;
		}
		return false;
	}

	std::string AppSpec::CommandLineArgs::Get(std::string_view arg, std::string_view default_value) const
	{
		if (arg.empty())
			return std::string(default_value);

		for (int x = 1; x < Count; ++x)
		{
			std::string_view raw_arg(Args[x]);
			std::string_view key, value;
			split_arg(raw_arg, key, value);

			if (key.empty())
				continue;

			if (ci_equal(key, arg))
			{
				if (!value.empty())
					return std::string(value);
				return std::string(default_value);
			}
		}
		return std::string(default_value);
	}


} // namespace Helios::Engine
