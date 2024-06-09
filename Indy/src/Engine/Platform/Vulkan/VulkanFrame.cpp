#include "Engine/Core/LogMacros.h"

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	void AllocateVulkanFrameData(const VkDevice& logicalDevice, const QueueFamilyIndices& queueFamilies, VulkanFrameData& frameData)
	{
		if (queueFamilies.compute.value() != queueFamilies.graphics.value())
			frameData.computeCmdPool.Allocate(logicalDevice, queueFamilies.compute.value(), 1);

		// Always create the graphics command pool
		frameData.graphicsCmdPool.Allocate(logicalDevice, queueFamilies.graphics.value(), 1);

		frameData.computeCmdBuffer = frameData.computeCmdPool.GetCommandBuffer(0);
		frameData.graphicsCmdBuffer = frameData.graphicsCmdPool.GetCommandBuffer(0);

		// Fences
		{
			VkFenceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			// Compute Fence
			if (vkCreateFence(logicalDevice, &createInfo, nullptr, &frameData.computeFence) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to create sync object [VkFence]!");
				return;
			}

			// Graphics Fence
			if (vkCreateFence(logicalDevice, &createInfo, nullptr, &frameData.graphicsFence) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to create sync object [VkFence]!");
				return;
			}
		}

		// Semaphores
		{
			VkSemaphoreCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;

			// Compute Semaphore
			if (vkCreateSemaphore(logicalDevice, &createInfo, nullptr, &frameData.computeSemaphore) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to create sync object [VkSemaphore]!");
				return;
			}

			// Graphics Semaphore
			if (vkCreateSemaphore(logicalDevice, &createInfo, nullptr, &frameData.graphicsSemaphore) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to create sync object [VkSemaphore]!");
				return;
			}

			// Swapchain Semaphore
			if (vkCreateSemaphore(logicalDevice, &createInfo, nullptr, &frameData.swapchainSemaphore) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to create sync object [VkSemaphore]!");
				return;
			}
		}
	}
}
