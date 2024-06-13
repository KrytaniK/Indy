module;

#include <string>
#include <memory>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Frame;

import :CommandPool;
import :Device;

export
{
	namespace Indy
	{
		struct VulkanFrameData
		{
			VulkanCommandPool computeCmdPool;
			VulkanCommandPool graphicsCmdPool;

			VkCommandBuffer computeCmdBuffer = VK_NULL_HANDLE;
			VkCommandBuffer graphicsCmdBuffer = VK_NULL_HANDLE;

			VkFence computeFence = VK_NULL_HANDLE;
			VkFence graphicsFence = VK_NULL_HANDLE;

			VkSemaphore computeSemaphore = VK_NULL_HANDLE;
			VkSemaphore graphicsSemaphore = VK_NULL_HANDLE;
			VkSemaphore swapchainSemaphore = VK_NULL_HANDLE;
		};

		void AllocateVulkanFrameData(const VkDevice& logicalDevice, const QueueFamilyIndices& queueFamilies, VulkanFrameData& frameData);
	}
}