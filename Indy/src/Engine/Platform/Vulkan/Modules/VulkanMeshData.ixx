module;

#include <glm/glm.hpp>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:MeshData;

import :Buffer;

import Indy.Graphics;

export
{
	namespace Indy
	{
		struct VulkanMeshData
		{
			static VulkanMeshData Create(const Mesh& mesh, const VkDevice& logicalDevice, const VmaAllocator& bufferAllocator);

			VulkanBuffer vertexBuffer = {};
			VulkanBuffer indexBuffer = {};
			VkDeviceAddress vertexBufferAddress = UINT64_MAX;
		};

		struct VulkanMeshPushConstants
		{
			glm::mat4 worldMatrix;
			VkDeviceAddress vertexBufferAddress = UINT64_MAX;
		};
	}
}