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
