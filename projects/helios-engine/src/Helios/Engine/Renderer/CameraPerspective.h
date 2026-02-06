//==============================================================================
// Perspective Camera
//
// Implements a perspective camera with controllable position, rotation, and
// projection parameters. It calculates the view and view-projection matrices
// required for 3D rendering.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
//
// Main Features:
// - Perspective projection for 3D rendering.
// - Controllable Field of View (FOV), aspect ratio, and clipping planes.
// - Position and rotation controls (yaw, pitch).
// - Automatic recalculation of the view-projection matrix.
//
// Changelog:
// - 2026.01: Initial version.
//==============================================================================
#pragma once

#include "Helios/Engine/Renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Helios::Engine::Renderer {


	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void SetProjection(float fov, float aspectRatio, float nearClip, float farClip);

		virtual const glm::vec3& GetPosition() const override { return m_position; }
		virtual void SetPosition(const glm::vec3& position) override { m_position = position; RecalculateViewMatrix(); }

		virtual const glm::vec3& GetRotation() const override { return m_rotation; }
		virtual void SetRotation(const glm::vec3& rotation) override { m_rotation = rotation; RecalculateViewMatrix(); }
		
		virtual const glm::mat4& GetViewMatrix() const override { return m_viewMatrix; }
		virtual const glm::mat4& GetViewProjectionMatrix() const override { return m_viewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();

	private:
		glm::mat4 m_viewMatrix;
		glm::mat4 m_viewProjectionMatrix;

		glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_rotation = { 0.0f, 0.0f, 0.0f }; // Yaw, Pitch, Roll
	};


} // namespace Helios::Engine::Renderer
