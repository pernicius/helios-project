//==============================================================================
// Windows Platform Utilities
//
// Provides Windows-specific utility functions for platform operations.
// Currently implements executable path detection using Windows API
// (GetModuleFileName). Additional platform-specific utilities can be added
// here as needed. Part of the cross-platform abstraction layer with
// equivalent implementations for Linux and macOS.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include <string>

namespace Helios::Util {


	std::string GetExecutablePath();


} // namespace Helios::Util
