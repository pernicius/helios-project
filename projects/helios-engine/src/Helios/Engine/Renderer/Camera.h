//==============================================================================
// Camera Abstraction
//
// Defines the abstract base class for cameras, providing a unified interface
// for managing view and projection transformations. This class decouples camera
// logic from the renderer and specific camera implementations (e.g.,
// orthographic, perspective).
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
//
// Main Features:
// - Abstract base class for different camera types.
// - Manages the projection matrix.
// - Provides an interface for retrieving view and projection matrices.
//
// Changelog:
// - 2026.01: Initial version.
//==============================================================================
#pragma once

#include <glm/glm.hpp>

namespace Helios::Engine::Renderer {


	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_projectionMatrix(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }

		virtual const glm::vec3& GetPosition() const = 0;
		virtual void SetPosition(const glm::vec3& position) = 0;

		virtual const glm::vec3& GetRotation() const = 0;
		virtual void SetRotation(const glm::vec3& rotation) = 0;

		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;

	protected:
		glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
	};


} // namespace Helios::Engine::Renderer
