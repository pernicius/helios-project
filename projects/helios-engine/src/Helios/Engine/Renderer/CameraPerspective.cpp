#include "pch.h"
#include "CameraPerspective.h"

namespace Helios::Engine::Renderer {


	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		RecalculateViewMatrix();
	}


	void PerspectiveCamera::SetProjection(float fov, float aspectRatio, float nearClip, float farClip)
	{
		m_projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}


	void PerspectiveCamera::RecalculateViewMatrix()
	{
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), m_rotation.x, { 1, 0, 0 })
			* glm::rotate(glm::mat4(1.0f), m_rotation.y, { 0, 1, 0 })
			* glm::rotate(glm::mat4(1.0f), m_rotation.z, { 0, 0, 1 });

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) * rotation;

		m_viewMatrix = glm::inverse(transform);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}


} // namespace Helios::Engine::Renderer
