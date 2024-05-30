#include "Engine/Core/LogMacros.h"

#include <string>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanFrame::VulkanFrame(VulkanDevice* device, const QueueFamilyIndices& queueFamilies)
	{
		m_CommandPool = std::make_unique<VulkanCommandPool>(device->Get(), queueFamilies.graphics.value(), 1);
		m_SyncObjects = std::make_unique<VulkanSyncObjects>(device->Get());

		m_SyncObjects->AddFence("Render", VK_FENCE_CREATE_SIGNALED_BIT);
		m_SyncObjects->AddSemaphore("Swapchain");
		m_SyncObjects->AddSemaphore("Render");
	}

	VulkanFrame::~VulkanFrame()
	{
		m_CommandPool = nullptr; // Destroy command pool first
		m_SyncObjects = nullptr;
	}


	VulkanCommandPool* VulkanFrame::GetCommandPool() const
	{
		return m_CommandPool.get();
	}

	const VkCommandBuffer& VulkanFrame::GetCommandBuffer(const uint8_t& index) const
	{
		return m_CommandPool->GetCommandBuffer(index);
	}

	const VkFence& VulkanFrame::GetFence(const std::string& fence) const
	{
		return m_SyncObjects->GetFence(fence);
	}

	const VkSemaphore& VulkanFrame::GetSemaphore(const std::string& semaphore) const
	{
		return m_SyncObjects->GetSemaphore(semaphore);
	}
}
