#include <Engine/Core/LogMacros.h>

#include <vma/vk_mem_alloc.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy
{
	VulkanMeshData VulkanMeshData::Create(const Mesh& mesh, const VkDevice& logicalDevice, const VmaAllocator& bufferAllocator)
	{
		// I'd like to note that this pattern isn't very efficient, as it blocks the main thread. It would be better to offload this
		//	to a separate thread, dedicated to mesh data uploading

		const size_t vBufferSize = mesh.vertices.size() * sizeof(Vertex);
		const size_t iBufferSize = mesh.indices.size() * sizeof(uint32_t);

		VulkanMeshData outData{};

		// Create Vertex Buffer
		outData.vertexBuffer = VulkanBuffer::Create(
			bufferAllocator, 
			vBufferSize, 
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VMA_MEMORY_USAGE_GPU_ONLY
		);

		// Get vertex buffer device address
		VkBufferDeviceAddressInfo deviceAddressInfo{};
		deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		deviceAddressInfo.buffer = outData.vertexBuffer.buffer;

		outData.vertexBufferAddress = vkGetBufferDeviceAddress(logicalDevice, &deviceAddressInfo);

		// Create Index Buffer
		outData.indexBuffer = VulkanBuffer::Create(
			bufferAllocator,
			iBufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_GPU_ONLY
		);

		// Create staging buffer to copy mesh data
		VulkanBuffer stagingBuffer = VulkanBuffer::Create(
			bufferAllocator, 
			vBufferSize + iBufferSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VMA_MEMORY_USAGE_CPU_ONLY
		);

		// Map staging buffer memory
		void* stageData = nullptr;
		vmaMapMemory(bufferAllocator, stagingBuffer.allocation, &stageData);

		// Copy vertex data
		memcpy(stageData, mesh.vertices.data(), vBufferSize);

		// Copy Index Data, positioned directly after vertex data
		memcpy(static_cast<char*>(stageData) + vBufferSize, mesh.indices.data(), iBufferSize);

		// Unmap staging buffer memory
		vmaUnmapMemory(bufferAllocator, stagingBuffer.allocation);

		// Copy buffers to GPU
		VulkanRenderer::SubmitImmediate([&](const VkCommandBuffer& cmdBuffer)
			{
				VkBufferCopy vertexCopy{ 0 };
				vertexCopy.dstOffset = 0;
				vertexCopy.srcOffset = 0;
				vertexCopy.size = vBufferSize;

				vkCmdCopyBuffer(cmdBuffer, stagingBuffer.buffer, outData.vertexBuffer.buffer, 1, &vertexCopy);

				VkBufferCopy indexCopy{ 0 };
				indexCopy.dstOffset = 0;
				indexCopy.srcOffset = vBufferSize;
				indexCopy.size = iBufferSize;

				vkCmdCopyBuffer(cmdBuffer, stagingBuffer.buffer, outData.indexBuffer.buffer, 1, &indexCopy);
			}
		);

		VulkanBuffer::Destroy(bufferAllocator, stagingBuffer);

		return outData;
	}
}