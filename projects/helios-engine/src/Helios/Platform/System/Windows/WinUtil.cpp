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
