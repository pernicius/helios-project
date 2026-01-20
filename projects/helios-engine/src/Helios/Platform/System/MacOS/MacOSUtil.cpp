#include "pch.h"
#include "Platform/System/MacOS/MacOSUtil.h"

#include "Platform/System/MacOS/MacOSMain.h"

#include <filesystem>
#include <mach-o/dyld.h>
#include <limits.h>
#include <string>

namespace Helios::Util {


	std::string GetExecutablePath() {
		char rawPathName[PATH_MAX];
		uint32_t rawPathSize = (uint32_t)sizeof(rawPathName);

		if (_NSGetExecutablePath(rawPathName, &rawPathSize) != 0)
			return {};

		std::error_code ec;
		std::filesystem::path exePath = std::filesystem::canonical(rawPathName, ec);
		if (ec)
			return {};

		return exePath.string();
	}


} // namespace Helios::Util
