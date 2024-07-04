#include <Engine/Core/LogMacros.h>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanBuffer VulkanBuffer::Create(const VmaAllocator& allocator, const size_t& size, const VkBufferUsageFlags& bufferUsage, const VmaMemoryUsage& memUsage)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = nullptr;
		bufferInfo.size = size;
		bufferInfo.usage = bufferUsage;
		
		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = memUsage;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
		
		VulkanBuffer outBuffer{};

		if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &outBuffer.buffer, &outBuffer.allocation, &outBuffer.allocInfo) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to allocate Vulkan Buffer!");
			return {};
		}

		return outBuffer;
	}

	void VulkanBuffer::Destroy(const VmaAllocator& allocator, const VulkanBuffer& buffer)
	{
		vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
	}
}