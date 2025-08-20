#include "pch.h"
#if defined(TARGET_PLATFORM_LINUX)
#include "Platform/System/Linux/LinuxMain.h"

#include "Helios/Engine/Core/Application.h"


int main(int argc, char** argv)
{
	// Call the engines main() function
	return Helios::AppMain(__argc, __argv);
}


#endif
