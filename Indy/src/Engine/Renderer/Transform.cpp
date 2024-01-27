#include "Transform.h"

#include "Engine/Core/LogMacros.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine
{
	void Transform::Translate(glm::vec3 translation, Space relativeTo)
	{
		switch (relativeTo)
		{
			case Space::World:
			{
				localToWorldMatrix = glm::translate(localToWorldMatrix, translation);
				return;
			};
			case Space::Local:
			{
				transformMatrix = glm::translate(transformMatrix, translation);
				return;
			};
		}
	}

	void Transform::Translate(float x, float y, float z, Space relativeTo)
	{
		switch (relativeTo)
		{
			case Space::World:
			{
				localToWorldMatrix = glm::translate(localToWorldMatrix, glm::vec3(x, y, z));
				return;
			};
			case Space::Local:
			{
				transformMatrix = glm::translate(transformMatrix, glm::vec3(x, y, z));
				return;
			};
		}
	}

	void Transform::Rotate(glm::quat rotation, Space relativeTo)
	{
		switch (relativeTo)
		{
			case Space::World:
			{
				localToWorldMatrix *= glm::mat4_cast(rotation);
				return;
			};
			case Space::Local:
			{
				transformMatrix *= glm::mat4_cast(rotation);
				return;
			};
		}
	}

	void Transform::Rotate(float pitch, float yaw, float roll, Space relativeTo)
	{
		switch (relativeTo)
		{
			case Space::World:
			{
				glm::quat xRot = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
				glm::quat yRot = glm::angleAxis(glm::radians(roll), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::quat zRot = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 0.0f, 1.0f));
				localToWorldMatrix *= glm::mat4_cast(glm::normalize(xRot * yRot * zRot));
				return;
			};
			case Space::Local:
			{
				glm::vec3 localXAxis = glm::vec3(transformMatrix[0][0], transformMatrix[1][0], transformMatrix[2][0]);
				glm::vec3 localYAxis = glm::vec3(transformMatrix[0][1], transformMatrix[1][1], transformMatrix[2][1]);
				glm::vec3 localZAxis = glm::vec3(transformMatrix[0][2], transformMatrix[1][2], transformMatrix[2][2]);

				glm::quat xRot = glm::angleAxis(glm::radians(pitch), localXAxis);
				glm::quat yRot = glm::angleAxis(glm::radians(yaw), localYAxis);
				glm::quat zRot = glm::angleAxis(glm::radians(roll), localZAxis);
				transformMatrix *= glm::mat4_cast(glm::normalize(xRot * yRot * zRot));
				return;
			};
		}
	}

	void Transform::Scale(glm::vec3 amount)
	{
		transformMatrix = glm::scale(transformMatrix, amount);
	}

	void Transform::Scale(float x, float y, float z)
	{
		transformMatrix = glm::scale(transformMatrix, glm::vec3(x, y, z));
	}

	void Transform::SetPosition(glm::vec3 newPosition)
	{
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1), newPosition);

		transformMatrix[3] = translationMatrix[3];
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPosition(glm::vec3(x, y, z));
	}

	void Transform::SetRotation(glm::quat quatRotation)
	{
		glm::mat4 rotationMatrix = glm::mat4_cast(quatRotation);

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				transformMatrix[i][j] = rotationMatrix[i][j];
			}
		}
	}

	void Transform::SetRotation(glm::vec3 eulerRotation)
	{
		SetRotation(glm::quat(eulerRotation));
	}

	void Transform::SetRotation(float pitch, float yaw, float roll)
	{
		SetRotation(glm::quat(glm::vec3(pitch, yaw, roll)));
	}

	glm::vec3 Transform::GetOrigin() const
	{
		return glm::vec3(localToWorldMatrix[3][0], localToWorldMatrix[3][1], localToWorldMatrix[3][2]);
	}

	glm::vec3 Transform::GetLocalPosition() const
	{
		return glm::vec3(transformMatrix[3][0], transformMatrix[3][1], transformMatrix[3][2]);
	}

	glm::vec3 Transform::GetWorldPosition() const
	{
		glm::mat4 worldTransformMatrix = localToWorldMatrix * transformMatrix;

		return glm::vec3(worldTransformMatrix[3][0], worldTransformMatrix[3][1], worldTransformMatrix[3][2]);
	}

	glm::quat Transform::GetLocalRotation() const
	{
		glm::mat3 rotationMatrix = glm::mat3(glm::normalize(transformMatrix[0]), glm::normalize(transformMatrix[1]), glm::normalize(transformMatrix[2]));

		return glm::quat_cast(rotationMatrix);
	}

	glm::quat Transform::GetWorldRotation() const
	{
		glm::mat4 worldTransformMatrix = transformMatrix * localToWorldMatrix;

		glm::mat3 rotationMatrix = glm::mat3(glm::normalize(worldTransformMatrix[0]), glm::normalize(worldTransformMatrix[1]), glm::normalize(worldTransformMatrix[2]));

		return glm::quat_cast(rotationMatrix);
	}

}