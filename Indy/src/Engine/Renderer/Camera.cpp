#include "Camera.h"

#include "Engine\Core\Log.h"

namespace Engine
{
	glm::mat4 Camera::GetProjectionMatrix(float aspectRatio)
	{
		projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClipPlane, farClipPlane);
		return projectionMatrix;
	}

	glm::mat4 Camera::GetViewMatrix()
	{
		viewMatrix = glm::translate(glm::mat4_cast(orientation), -position);
		return viewMatrix;
	}

	void Camera::move(float forward, float strafe, float up)
	{
		position.x += strafe;
		position.y += forward;
		position.z += up;
	}

	void Camera::rotate(float pitch, float yaw)
	{
		glm::quat pitchRotation = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat yawRotation = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

		orientation = glm::normalize(yawRotation * pitchRotation * orientation);
	}
}