//==============================================================================
// Windows Platform Main Entry Point (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file WinMain.h
//==============================================================================
#include "pch.h"
#include "Helios/Platform/System/Windows/WinMain.h"

#include "Helios/Engine/Core/Application.h"


#ifdef BUILD_DEBUG
#	pragma message ("INFO: Compiling stdc main() for console debugging output!")
	// ConsoleApp for stdout debuging
	int main(int argc, char** argv)
#else
#	pragma message ("INFO: Compiling windows WinMain() for non-debugging release!")
	// WindowedApp
	int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
#endif
	{
		// Call the engines main() function
		return Helios::Engine::AppMain(__argc, __argv);
	}
