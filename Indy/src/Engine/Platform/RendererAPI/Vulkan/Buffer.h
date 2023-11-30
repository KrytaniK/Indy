#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Engine::VulkanAPI
{
	class Buffer
	{
	public:
		static void CopyBuffer(VkBuffer src, VkBuffer dst, const VkDeviceSize& size);

	public:
		Buffer() {};

		// Constructor for staging buffers
		Buffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties);

		// Constructor for regular buffers
		Buffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, const bool persistentMapping);

		~Buffer();

		void MapMemory(const VkDeviceSize& dataSize, const void* srcData);
		void UnmapMemory();

		void WriteToMappedMemory(const void* srcData, const VkDeviceSize& dataSize, const VkDeviceSize& offset);

		VkBuffer GetBuffer() const { return m_Buffer; };
		VkDeviceSize GetBufferSize() const { return m_BufferSize; };

	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
		VkDeviceSize m_BufferSize;
		void* m_BufferMapped;
		bool m_PersistentMapping;
	};
}