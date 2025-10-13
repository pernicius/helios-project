#include "pch.h"
#include "Platform/System/Windows/WinUtil.h"

#include "Platform/System/Windows/WinMain.h"

namespace Helios::Util {


    std::string GetExecutablePath()
    {
        std::string path;
        char ctemp[MAX_PATH];
        wchar_t wtemp[MAX_PATH];
 
        GetModuleFileName(NULL, wtemp, MAX_PATH);
        wcstombs_s(nullptr, ctemp, wtemp, MAX_PATH);
 
        std::string strtemp(ctemp);
        path = strtemp.substr(0, strtemp.find_last_of("/\\"));
 
        return path;
    }


} // namespace Helios::Util
