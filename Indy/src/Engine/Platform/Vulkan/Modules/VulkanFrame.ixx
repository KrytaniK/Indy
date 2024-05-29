module;

#include <string>
#include <memory>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Frame;

import :Queue;
import :CommandPool;
import :SyncObjects;

export
{
	namespace Indy
	{
		class VulkanFrame
		{
		public:
			VulkanFrame(const VkDevice& logicalDevice, const QueueFamilyIndices& queueFamilies);
			~VulkanFrame();

			VulkanCommandPool* GetCommandPool() const;
			const VkCommandBuffer& GetCommandBuffer(const uint8_t& index) const;

			const VkFence& GetFence(const std::string& fence) const;
			const VkSemaphore& GetSemaphore(const std::string& semaphore) const;

		private:
			std::unique_ptr<VulkanCommandPool> m_CommandPool;
			std::unique_ptr<VulkanSyncObjects> m_SyncObjects;
		};
	}
}