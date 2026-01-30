#pragma once

//==============================================================================
// Windows Platform Main Header
//
// Provides Windows-specific platform initialization and configuration. Defines
// NOMINMAX to prevent conflicts between Windows min/max macros and standard
// library functions. Includes windows.h with proper preprocessor guards.
// Required for all Windows platform code to ensure consistent macro
// definitions and header inclusion order.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Changelog:
// - 2026.01: Initial version / start of version history
//==============================================================================

// On Windows, a header file defines two macros min and max which may result in conflicts
// with their counterparts in the standard library and therefore in errors during compilation.
// See https://github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#ifndef NOMINMAX
#	define NOMINMAX
#endif

// default includes
#include <windows.h>
