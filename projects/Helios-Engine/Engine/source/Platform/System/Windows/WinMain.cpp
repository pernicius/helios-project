#include "pch.h"
#include "Platform/System/Windows/WinMain.h"

#include "Helios/Engine/Core/Application.h"


#ifdef BUILD_DEBUG

// ConsoleApp for stdout debuging
int main(int argc, char** argv)

#else

// WindowedApp
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)

#endif

{
	// Call the engines main() function
	return Helios::AppMain(__argc, __argv);
}
