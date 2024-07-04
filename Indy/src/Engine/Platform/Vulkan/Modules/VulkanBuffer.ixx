module;

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

export module Indy.VulkanGraphics:Buffer;

export
{
	namespace Indy
	{
		struct VulkanBuffer
		{
			static VulkanBuffer Create(const VmaAllocator& allocator, const size_t& size, const VkBufferUsageFlags& bufferUsage, const VmaMemoryUsage& memUsage);
			static void Destroy(const VmaAllocator& allocator, const VulkanBuffer& buffer);

			VkBuffer buffer = VK_NULL_HANDLE;
			VmaAllocation allocation = VK_NULL_HANDLE;
			VmaAllocationInfo allocInfo = {};
		};
	}
}