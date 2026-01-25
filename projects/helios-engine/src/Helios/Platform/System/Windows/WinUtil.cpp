//==============================================================================
// Windows Platform Utilities (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file WinUtil.h
//==============================================================================
#include "pch.h"
#include "Helios/Platform/System/Windows/WinUtil.h"

#include "Helios/Platform/System/Windows/WinMain.h"

#include <filesystem>

namespace Helios::Util {


	std::string GetExecutablePath()
	{
		wchar_t wtemp[MAX_PATH] = { 0 };
		if (GetModuleFileNameW(NULL, wtemp, MAX_PATH) == 0) {
			// TODO: error handling
			return {};
		}

		std::filesystem::path exePath(wtemp);
		return exePath.parent_path().string();
	}


} // namespace Helios::Util
