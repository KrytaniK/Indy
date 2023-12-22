#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	enum Space
	{
		World = 0, Local = 1
	};

	struct Transform
	{
		void Translate(glm::vec3 translation, Space relativeTo);
		void Translate(float x, float y, float z, Space relativeTo);

		void Rotate(glm::quat quatRotation, Space relativeTo);
		void Rotate(float pitch, float yaw, float roll, Space relativeTo);

		void Scale(glm::vec3 amount);
		void Scale(float x, float y, float z);

		void SetPosition(glm::vec3 newPosition);
		void SetPosition(float x, float y, float z);

		void SetRotation(glm::quat newRotation);
		void SetRotation(glm::vec3 newRotation);
		void SetRotation(float pitch, float yaw, float roll);

		glm::vec3 GetLocalPosition() const;
		glm::vec3 GetWorldPosition() const;
		glm::quat GetLocalRotation() const;
		glm::quat GetWorldRotation() const;

	private:

		// Represents this transform's "origin" transform. This would be the transformMatrix of the parent transform, or the world transform.
		glm::mat4 localToWorldMatrix = glm::mat4(1); 

		// Represents this transform's transformations (translation, rotation, scale)
		glm::mat4 transformMatrix = glm::mat4(1);
	};
}