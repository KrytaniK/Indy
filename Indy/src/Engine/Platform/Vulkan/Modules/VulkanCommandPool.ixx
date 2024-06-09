module;

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:CommandPool;

export
{
	namespace Indy
	{
		class VulkanCommandPool
		{
		public:
			VulkanCommandPool() = default;
			~VulkanCommandPool();

			const VkCommandPool& Get() const { return m_CommandPool; };
			const VkCommandBuffer& GetCommandBuffer(const uint8_t& index) const;

			void Allocate(const VkDevice& logicalDevice, const uint32_t& queueFamilyIndex, const uint8_t& commandBufferCount);

			void BeginCommandBuffer(const uint8_t& index, const VkCommandBufferUsageFlags& flags);
			void EndCommandBuffer(const uint8_t& index);

			void Reset();

		private:
			void AllocateCommandBuffers(const uint8_t& commandBufferCount);

		private:
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			VkCommandPool m_CommandPool = VK_NULL_HANDLE;
			std::vector<VkCommandBuffer> m_CommandBuffers;
			VkCommandBufferBeginInfo m_CmdBufferBeginInfo{};
		};
	}
}