#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Engine
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
	};
}