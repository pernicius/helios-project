#include "pch.h"
#include "Platform/System/Linux/LinuxUtil.h"

#include "Platform/System/Linux/LinuxMain.h"

#include <limits.h>
#include <libgen.h>
#include <unistd.h>

#if defined(__sun)
#   define PROC_SELF_EXE "/proc/self/path/a.out"
#else
#   define PROC_SELF_EXE "/proc/self/exe"
#endif

namespace Helios::Util {


    std::string GetExecutablePath() {
        char rawPathName[PATH_MAX];
        realpath(PROC_SELF_EXE, rawPathName);
        return  std::string(rawPathName);
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
