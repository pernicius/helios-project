# Contributing to Helios Project

## Overview
These guidelines describe the coding style and contribution process used across the Helios Project. They reflect the conventions enforced by the repository's .editorconfig and existing source layout.

## Coding Style
- Use tabs for indentation (match .editorconfig). Do not mix spaces.
- Use Unix-style line endings (LF) and ensure each file ends with a newline.
- File headers should start with `#include "pch.h"` when precompiled headers are present.
- Use the `Helios::Engine::{...}` namespace hierarchy for engine code and `Helios::Engine::Renderer::Vulkan` for Vulkan-specific implementation.
- Types and classes: PascalCase (e.g., `VKRenderPass`, `DeviceManager`, `ConfigSection`).
- Functions and methods: PascalCase (e.g., `Create`, `CreateFramebuffer`, `GetSection`, `LoadDomain`).
- Member variables: **use `m_` prefix** for all private/protected members (e.g., `m_data`, `m_filePath`, `m_sections`, `m_domains`). This is the standard convention across the codebase.
- Use `Ref<T>/CreateRef<T>()` for managed references where appropriate.
- Avoid language features that reduce portability across supported toolchains without justification.

## Formatting
- Follow .editorconfig rules exactly.
- Keep line lengths reasonable (wrap at ~120 characters where necessary).
- Order includes: PCH header first (`#include "pch.h"`), then project headers, then standard library headers, with blank lines between groups.
- **Braces**: Use Egyptian/K&R style — opening brace on same line as control statement, closing brace on new line:
  ```cpp
  if (condition) {
      // code
  }
  
  for (const auto& item : collection) {
      // code
  }
  ```
- **Class sections**: Separate major implementation sections with comment dividers:
  ```cpp
  //------------------------------------------------------------------------------
  // ConfigSection Implementation
  //------------------------------------------------------------------------------
  ```
- **Blank lines**: Add blank lines around function implementations for readability. Keep template instantiations grouped at the end of implementation files.
- **Templates**: For template classes/functions, provide explicit instantiations at the end of the `.cpp` file for commonly-used types.

## File Headers
All source files must include a standardized header comment block at the top of the file. This ensures consistent documentation and copyright information across the project.

### Header Files (.h)
Use the full header format with double-line separators (`==============================================================================`):

```cpp
//==============================================================================
// Brief Title (e.g., "Application Core")
//
// Detailed description of the file's purpose, functionality, and key features.
// This section should explain what the file provides, how it fits into the
// engine architecture, and any important implementation details or usage notes.
// Multiple paragraphs are acceptable for complex modules.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once
```

**Guidelines:**
- **Title**: Short, descriptive name of the component (e.g., "Application Core", "INI File Parser", "Debugger Break Utility")
- **Description**: Comprehensive explanation of purpose, features, and architecture. Describe what problems it solves and how it integrates with other systems.
- **Copyright**: Use current year and standard copyright notice format
- **Project URL**: Always include the GitHub repository link
- **Version history**: Start with initial version entry; add new entries for major changes
- **Separator lines**: Exactly 78 equal signs (`=`)

### Implementation Files (.cpp)
Use the simplified header format that references the corresponding header:

```cpp
//==============================================================================
// Brief Title (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file FileName.h
//==============================================================================
#include "pch.h"
```

**Guidelines:**
- Add "(implementation)" suffix to the title
- No detailed description needed (refer to header file instead)
- Include reference line pointing to the header file
- First `#include` after header must be `"pch.h"` when PCH is enabled

### Special Cases

#### Third-party Code
When integrating third-party code, preserve original license information and add Helios Project context:

```cpp
//==============================================================================
// Component Name
//
// Brief description of functionality.
//
// Third-party code: [source URL]
// Licensed under [License Name] (see below)
// Modified for integration into Helios Project
//
// Part of the Helios Project - https://github.com/pernicius/helios-project
//==============================================================================
// [Original copyright and license text follows]
```

#### Platform-Specific Headers
Platform headers should note their platform scope in the title:

```cpp
//==============================================================================
// Windows Platform Main Header
//
// Description of platform-specific functionality...
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
```

### Version History Format
Maintain a chronological version history (newest first) with brief descriptions:

```cpp
// Version history:
// - 2026.03: Performance improvements, bug fixes
// - 2026.02: Added feature X, refactored Y
// - 2026.01: Initial version / start of version history
```

Use year.month format for version entries. Keep descriptions concise but informative.

## Code Organization
- Use forward declarations and friend classes where appropriate to maintain encapsulation while allowing necessary access.
- Keep helper functions and compile-time constants in private sections.
- Group related methods together (e.g., getters, setters, loaders, savers).

## PRs and Commits
- Write focused commits with descriptive messages.
- Open a PR per feature/fix with a short description and list of changes.

## Tests and Validation
- Provide unit tests for new logic where feasible.
- Ensure the project builds on the supported platforms and configurations.

## Files
This file documents the project's expected style and will be used to guide automated formatting and code reviews.

## Engine Architecture & File Organization
This section describes recommended module layout and conventions for engine code, including Vulkan-specific guidance. These recommendations are intended to keep the codebase modular, testable, and consistent across platforms.

### High-level modules
- `Core` — application lifecycle, platform-agnostic initialization, event system, main loop, configuration management. Place under `projects/helios-engine/src/Helios/Engine/Core/`.
- `Renderer` — renderer-facing API and abstractions (Renderer, Window, Format, DeviceManager). Place platform-agnostic interfaces under `projects/helios-engine/src/Helios/Engine/Renderer/`.
- `Platform` — platform-specific implementations (Vulkan, DirectX, Metal). Each backend should live under `projects/helios-engine/src/Helios/Platform/` with subfolders per backend, e.g. `Platform/Renderer/Vulkan/`.
- `Util` — small helpers, parsers, file loaders (e.g., `projects/helios-engine/src/Helios/Engine/Util/`).
- `Resources` — asset loading, shaders, textures, meshes.
- `Scene`/`ECS` — entity/component system and scene management.
- `Vendor` — third-party libraries under `vendor/` and referenced via project files.

### Directory layout example for Vulkan backend
- `projects/helios-engine/src/Helios/Platform/Renderer/Vulkan/VKInstance.*`
- `projects/helios-engine/src/Helios/Platform/Renderer/Vulkan/VKDevice*`
- `projects/helios-engine/src/Helios/Platform/Renderer/Vulkan/VKSwapchain*`
- `projects/helios-engine/src/Helios/Platform/Renderer/Vulkan/VKPipeline*`
- `projects/helios-engine/src/Helios/Platform/Renderer/Vulkan/VKRenderPass*`
- `projects/helios-engine/src/Helios/Platform/Renderer/Vulkan/VKWindow*`
- `projects/helios-engine/src/Helios/Platform/Renderer/Vulkan/Misc.h` (small helpers)

Keep platform-specific code out of `Helios::Engine` public headers; expose only stable, platform-agnostic interfaces there.

### Naming & API rules for Vulkan code
- Prefix Vulkan-specific classes with `VK` (e.g., `VKDevice`, `VKSwapchain`, `VKPipeline`).
- Keep the `Helios::Platform::Renderer::Vulkan` namespace for implementation details; put shared renderer interfaces in `Helios::Engine::Renderer`.
- Use RAII wrappers for Vulkan handles and explicit teardown functions for complex objects.
- Centralize Vulkan initialization and feature/extension querying in `VKInstance` and `VKDevice` abstractions.

### Build and project layout
- One VS project per logical component is acceptable (engine core, renderer backend, sandbox app). Keep third-party libs in `vendor/` with their own projects.
- Export minimal headers for engine consumers; prefer pimpl or header-only facade to reduce compile times.
- Use precompiled headers for engine projects; include `#include "pch.h"` at the top of source files when enabled.

### Tests and Validation
- Add unit tests for platform-agnostic logic in separate test projects.
- For renderer/backends, add small integration tests that smoke-initialize the API (create instance, device, and swapchain) in CI if feasible.

### Misc
- Document any new architectural decisions in `docs/` or the repository README.
- When adding or changing formatting or coding rules, update this file and `.editorconfig` accordingly.
