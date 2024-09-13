module;

#include <vector>
#include <cstdint>

#include <glm/glm.hpp>

export module Indy.Graphics:MeshData;

export
{
	namespace Indy
	{
		struct Vertex
		{
			glm::vec3 position;
			float uv_x;
			glm::vec3 normal;
			float uv_y;
			glm::vec4 color;
		};

		struct Mesh
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
		};
	}
}