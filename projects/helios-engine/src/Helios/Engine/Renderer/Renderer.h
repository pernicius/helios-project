//==============================================================================
// Renderer Interface (abstraction)
//
// Defines the abstract base class for the graphics renderer, providing a unified
// interface for all rendering operations. This class decouples the main
// application from the underlying graphics API (e.g., Vulkan, DirectX) by
// defining a contract that concrete renderer implementations must follow.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Main Features:
// - Abstract Base Class: Defines the pure virtual functions for the renderer.
// - API-Agnostic Interface: Hides API-specific details from the application.
// - Static Factory Method: Provides a single `Create()` entry point to
//   instantiate the correct renderer based on the selected RendererAPI.
// - Lifecycle Management: `Init()` and `Shutdown()` methods manage the
//   renderer's state and resources.
// - Extensible Design: Structured to accommodate future rendering commands
//   like `BeginFrame`, `Draw`, and `EndFrame`.
// 
// Changelog:
// - 2026.01: Refactored rendering loop into BeginFrame/EndFrame
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include "Helios/Engine/Core/Application.h"
#include "Helios/Engine/Renderer/Window.h"

namespace Helios::Engine::Renderer {


	class Renderer
	{
	public:
		static Scope<Renderer> Create();
		virtual ~Renderer() = default;

		virtual void Init(const AppSpec& appSpec, Window& window) = 0;
		virtual void Shutdown() = 0;

		virtual void OnEvent(Event& e) = 0;
		
		virtual bool BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void DrawFrame() = 0;

		// Other methods for rendering will go here later
		// virtual void Draw() = 0;
	};


} // namespace Helios::Engine::Renderer
