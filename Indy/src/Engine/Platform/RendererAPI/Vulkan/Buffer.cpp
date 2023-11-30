#include "Buffer.h"

#include "Engine/Core/Log.h"
#include "Device.h"
#include "CommandPool.h"

namespace Engine::VulkanAPI
{
	Buffer::Buffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties)
		: m_BufferSize(size), m_PersistentMapping(false), m_Buffer(nullptr), m_BufferMemory(nullptr), m_BufferMapped(nullptr)
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
			throw std::runtime_error("[Vulkan Buffer] Failed to create vertex buffer!");

		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(logicalDevice, m_Buffer, &memReqs);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memReqs.size;
		allocInfo.memoryTypeIndex = Device::FindMemoryType(memReqs.memoryTypeBits, properties); // visible and coherent bits allow the mapping of this memory

		if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("[Vulkan Buffer] Failed to allocate buffer memory!");
		}

		// Bind the vertex buffer to its memory
		vkBindBufferMemory(logicalDevice, m_Buffer, m_BufferMemory, 0);
	}

	Buffer::Buffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, const bool persistentMapping)
		: m_BufferSize(size), m_PersistentMapping(persistentMapping), m_Buffer(nullptr), m_BufferMemory(nullptr), m_BufferMapped(nullptr)
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
			throw std::runtime_error("[Vulkan Buffer] Failed to create vertex buffer!");

			VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(logicalDevice, m_Buffer, &memReqs);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memReqs.size;
		allocInfo.memoryTypeIndex = Device::FindMemoryType(memReqs.memoryTypeBits, properties); // visible and coherent bits allow the mapping of this memory

		if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		// Bind the vertex buffer to its memory
		vkBindBufferMemory(logicalDevice, m_Buffer, m_BufferMemory, 0);
	}

	Buffer::~Buffer()
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();

		vkDestroyBuffer(logicalDevice, m_Buffer, nullptr);
		vkFreeMemory(logicalDevice, m_BufferMemory, nullptr);
	}

	void Buffer::MapMemory(const VkDeviceSize& dataSize, const void* srcData)
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();

		if (m_PersistentMapping)
		{
			vkMapMemory(logicalDevice, m_BufferMemory, 0, dataSize, 0, &m_BufferMapped);
			return;
		}

		void* tempData = nullptr;
		vkMapMemory(logicalDevice, m_BufferMemory, 0, dataSize, 0, &tempData);
		memcpy(tempData, srcData, (size_t)dataSize);
		this->UnmapMemory();
	}

	void Buffer::UnmapMemory()
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();

		vkUnmapMemory(logicalDevice, m_BufferMemory);
	}

	void Buffer::WriteToMappedMemory(const void* srcData, const VkDeviceSize& dataSize, const VkDeviceSize& offset)
	{
		if (!m_PersistentMapping)
			throw std::runtime_error("[Vulkan Buffer] Attempted to write to a buffer that is not persistent!");

		if (dataSize + offset > m_BufferSize)
		{
			INDY_CORE_CRITICAL("[Vulkan Buffer] Attempted write to persistent buffer with [size: {0}, offset: {1}] exceeds buffer size of {2}!", dataSize, offset, m_BufferSize);
			throw std::runtime_error("Attempted to write to a buffer that isn't large enough!");
		}

		void* tempData = static_cast<char*>(m_BufferMapped) + offset;
		memcpy(tempData, srcData, dataSize);
	}

	void Buffer::CopyBuffer(VkBuffer src, VkBuffer dst, const VkDeviceSize& size)
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();
		const Queue& graphicsQueue = Device::GetGraphicsQueue();
		const VkCommandPool& commandPool = CommandPool::GetCommandPool();

		// Allocate a temporary command buffer
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer tempCmdBuffer;
		vkAllocateCommandBuffers(logicalDevice, &allocInfo, &tempCmdBuffer);

		// Record the copy commands
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(tempCmdBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;

		// Copy buffer
		vkCmdCopyBuffer(tempCmdBuffer, src, dst, 1, &copyRegion);

		// Stop recording
		vkEndCommandBuffer(tempCmdBuffer);

		// Submit copy commands
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &tempCmdBuffer;

		vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue.queue);

		// cleanup temp command buffer
		vkFreeCommandBuffers(logicalDevice, commandPool, 1, &tempCmdBuffer);
	}
}