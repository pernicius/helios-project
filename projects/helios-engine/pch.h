//==============================================================================
// Precompiled Header (Engine)
//
// Aggregates frequently-used engine headers for precompilation to improve
// build times. Includes the master Engine.h header which provides the complete
// engine API surface. This file is compiled once and reused across all engine
// translation units. Keep expensive-to-parse headers here; avoid frequently
// changing headers to maximize cache effectiveness.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Changelog:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include "Helios/Engine/Engine.h"
