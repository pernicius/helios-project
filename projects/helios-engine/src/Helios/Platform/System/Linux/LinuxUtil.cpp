#include "pch.h"
#include "Platform/System/Linux/LinuxUtil.h"

#include <unistd.h>
#include <limits.h>
#include <string>
#include <filesystem>

namespace Helios::Util {


	std::string GetExecutablePath()
	{
		char result[PATH_MAX] = { 0 };
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX - 1);
		if (count == -1) {
			// TODO: error handling
			return {};
		}

		std::filesystem::path exePath(result);
		return exePath.parent_path().string();
	}


} // namespace Helios::Util
