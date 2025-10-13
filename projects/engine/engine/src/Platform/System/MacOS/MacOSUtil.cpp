#include "pch.h"
#include "Platform/System/MacOS/MacOSUtil.h"

#include "Platform/System/MacOS/MacOSMain.h"

#include <libgen.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <unistd.h>

namespace Helios::Util {


    std::string GetExecutablePath() {
        char rawPathName[PATH_MAX];
        char realPathName[PATH_MAX];
        uint32_t rawPathSize = (uint32_t)sizeof(rawPathName);

        if (!_NSGetExecutablePath(rawPathName, &rawPathSize)) {
            realpath(rawPathName, realPathName);
        }
        return  std::string(realPathName);
    }


    std::string GetExecutableDir() {
        std::string executablePath = GetExecutablePath();
        char* executablePathStr = new char[executablePath.length() + 1];
        strcpy(executablePathStr, executablePath.c_str());
        char* executableDir = dirname(executablePathStr);
        delete[] executablePathStr;
        return std::string(executableDir);
    }

}
