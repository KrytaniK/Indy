module;

#include <string>
#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

export module Indy.Graphics:Camera;

import :Types;

export
{
	namespace Indy::Graphics
	{
		// The camera represents the "eyes" of an entity with regard to the game
		// world.
		// TODO: Implement
		struct Camera
		{
			//static Camera Create(const glm::vec3& startPos, const glm::quat& startRot);

			// Transform transform;

			glm::mat4 view;
			glm::mat4 projection;
			glm::mat4 viewProjection;

			uint32_t id = UINT32_MAX;
			float fieldOfView = 80.0f;
			float nearClipPlane = 0.1f;
			float farClipPlane = 10.0f;
		};
	}
}