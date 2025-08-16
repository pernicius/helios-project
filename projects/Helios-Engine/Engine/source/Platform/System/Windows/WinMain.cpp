#include "Helios/Engine/Core/Application.h"
#include "WinMain.h"


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
