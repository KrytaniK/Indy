module;

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:CommandPool;

/* Note:
 *	If commands need to be executed via multiple threads, it is best to create multiple
 *		command pools per frame (one for each thread of access). and reset the entire
 *		command pool once safe.
 */

export
{
	namespace Indy
	{
		class VulkanCommandPool
		{
		public:
			VulkanCommandPool(const VkDevice& logicalDevice, const uint32_t& queueFamilyIndex, const uint8_t& commandBufferCount);
			~VulkanCommandPool();

			const VkCommandPool& Get() const { return m_CommandPool; };
			const VkCommandBuffer& GetCommandBuffer(const uint8_t& index) const;

			void BeginCommandBuffer(const uint8_t& index, const VkCommandBufferUsageFlags& flags) const;
			void EndCommandBuffer(const uint8_t& index) const;

			void Reset(bool release = false) const;

		private:
			void AllocateCommandBuffers(const uint8_t& commandBufferCount);

		private:
			VkDevice m_LogicalDevice;
			VkCommandPool m_CommandPool;
			std::vector<VkCommandBuffer> m_CommandBuffers;
		};
	}
}