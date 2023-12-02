#pragma once

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Engine
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
	};

	struct Geometry
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

	struct Mesh
	{
		std::vector<Geometry> geometries;
		glm::mat4 model;
	};
}