module;

#include <string>
#include <memory>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Frame;

import :Queue;
import :CommandPool;
import :SyncObjects;
import :Image;
import :Device;

export
{
	namespace Indy
	{
		struct FrameData
		{
			VulkanCommandPool* computeCommandPool;
			VulkanCommandPool* graphicsCommandPool;
			VkCommandBuffer computeCommandBuffer;
			VkCommandBuffer graphicsCommandBuffer;
			VkFence computeFence;
			VkFence renderFence;
			VkSemaphore computeSemaphore;
			VkSemaphore renderSemaphore;
			VkSemaphore imageAvailableSemaphore;
			VkQueue computeQueue;
			VkQueue graphicsQueue;
			uint32_t swapchainImageIndex;
		};

		class VulkanFrame
		{
		public:
			VulkanFrame(VulkanDevice* device, const QueueFamilyIndices& queueFamilies);
			~VulkanFrame();

			VulkanCommandPool* GetComputeCommandPool() const;
			VulkanCommandPool* GetGraphicsCommandPool() const;
			const VkCommandBuffer& GetComputeCommandBuffer(const uint8_t& index) const;
			const VkCommandBuffer& GetGraphicsCommandBuffer(const uint8_t& index) const;

			const VkFence& GetFence(const std::string& fence) const;
			const VkSemaphore& GetSemaphore(const std::string& semaphore) const;

		private:
			std::unique_ptr<VulkanCommandPool> m_ComputeCommandPool;
			std::unique_ptr<VulkanCommandPool> m_GraphicsCommandPool;
			std::unique_ptr<VulkanSyncObjects> m_SyncObjects;
		};
	}
}