#pragma once

#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine
{
	struct Camera
	{
		glm::mat4 GetProjectionMatrix(float aspectRatio)
		{
			projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClipPlane, farClipPlane);
			return projectionMatrix;
		}

		glm::mat4 GetViewMatrix()
		{
			viewMatrix = glm::translate(glm::mat4_cast(transform.GetWorldRotation()), -transform.GetWorldPosition());
			return viewMatrix;
		}

		Transform transform;
	private:
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;

		float fov = 65.0f;
		float nearClipPlane = 0.1f;
		float farClipPlane = 10.0f;
	};
}