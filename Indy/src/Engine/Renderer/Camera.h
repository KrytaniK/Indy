#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine
{
	struct Camera
	{
		glm::mat4 GetProjectionMatrix(float aspectRatio);

		glm::mat4 GetViewMatrix();

		void move(float forward, float strafe, float up);
		void rotate(float pitch, float yaw);

	private:
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;

		glm::vec3 position = {0.0f, 0.0f, 0.0f};
		glm::quat orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

		float fov = 65.0f;
		float nearClipPlane = 0.1f;
		float farClipPlane = 10.0f;
	};
}