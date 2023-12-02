#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Engine::VulkanAPI
{
	class Buffer
	{

	public:
		Buffer() {};

		// Constructor for staging buffers
		Buffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties);

		~Buffer();

		void Map(const void* srcData, uint32_t dataSize);
		void Map(); // <-- Persistent Mapping
		void Unmap();

		void Write(const void* srcData, const VkDeviceSize& dataSize, const VkDeviceSize& offset);
		void CopyTo(VkBuffer dst);

		VkBuffer Get() const { return m_Buffer; };
		VkDeviceSize GetSize() const { return m_BufferSize; };

	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
		VkDeviceSize m_BufferSize;
		void* m_BufferMapped;
		bool m_PersistentMapping;
	};
}