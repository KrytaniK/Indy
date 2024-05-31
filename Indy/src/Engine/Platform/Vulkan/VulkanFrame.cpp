#include "Engine/Core/LogMacros.h"

#include <string>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanFrame::VulkanFrame(VulkanDevice* device, const QueueFamilyIndices& queueFamilies)
	{
		// Create a dedicated compute command pool if there is a dedicated compute queue
		if (queueFamilies.compute.value() != queueFamilies.graphics.value())
			m_ComputeCommandPool = std::make_unique<VulkanCommandPool>(device->Get(), queueFamilies.compute.value(), 1);

		// Always create the graphics command pool
		m_GraphicsCommandPool = std::make_unique<VulkanCommandPool>(device->Get(), queueFamilies.graphics.value(), 1);

		m_SyncObjects = std::make_unique<VulkanSyncObjects>(device->Get());

		m_SyncObjects->AddFence("Render", VK_FENCE_CREATE_SIGNALED_BIT);
		m_SyncObjects->AddFence("Compute", VK_FENCE_CREATE_SIGNALED_BIT);
		m_SyncObjects->AddSemaphore("Swapchain");
		m_SyncObjects->AddSemaphore("Render");
		m_SyncObjects->AddSemaphore("Compute");
	}

	VulkanFrame::~VulkanFrame()
	{
		// Destroy command pools first
		m_ComputeCommandPool = nullptr; 
		m_GraphicsCommandPool = nullptr;

		m_SyncObjects = nullptr;
	}

	VulkanCommandPool* VulkanFrame::GetComputeCommandPool() const
	{
		if (!m_ComputeCommandPool)
			return m_GraphicsCommandPool.get();

		return m_ComputeCommandPool.get();
	}
	
	VulkanCommandPool* VulkanFrame::GetGraphicsCommandPool() const
	{
		return m_GraphicsCommandPool.get();
	}

	const VkCommandBuffer& VulkanFrame::GetComputeCommandBuffer(const uint8_t& index) const
	{
		return m_ComputeCommandPool->GetCommandBuffer(index);
	}

	const VkCommandBuffer& VulkanFrame::GetGraphicsCommandBuffer(const uint8_t& index) const
	{
		return m_GraphicsCommandPool->GetCommandBuffer(index);
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
