#include "pch.h"
#include "Platform/System/MacOS/MacOSMain.h"

#include "Helios/Engine/Core/Application.h"


int main(int argc, char** argv)
{
	// Call the engines main() function
	return Helios::Engine::AppMain(__argc, __argv);
}
