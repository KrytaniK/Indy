#include <Engine/Core/LogMacros.h>

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanCommandPool::VulkanCommandPool(const VkDevice& logicalDevice, const uint32_t& queueFamilyIndex, const uint8_t& commandBufferCount)
	{
		m_LogicalDevice = logicalDevice;

		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = queueFamilyIndex;
		createInfo.pNext = nullptr;

		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; 

		if (vkCreateCommandPool(m_LogicalDevice, &createInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create command pool!");
			return;
		}

		AllocateCommandBuffers(commandBufferCount);
	}

	VulkanCommandPool::~VulkanCommandPool()
	{
		// Then, destroy the command pool and allocated buffers
		vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
	}

	void VulkanCommandPool::AllocateCommandBuffers(const uint8_t& commandBufferCount)
	{
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = m_CommandPool;
		allocateInfo.commandBufferCount = commandBufferCount;
		allocateInfo.pNext = nullptr;

		m_CommandBuffers.resize(commandBufferCount);

		if (vkAllocateCommandBuffers(m_LogicalDevice, &allocateInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create command buffers!");
			return;
		}
	}

	const VkCommandBuffer& VulkanCommandPool::GetCommandBuffer(const uint8_t& index) const
	{
		if (index >= m_CommandBuffers.size())
			return VK_NULL_HANDLE;

		return m_CommandBuffers[index];
	}

	void VulkanCommandPool::BeginCommandBuffer(const uint8_t& index, const VkCommandBufferUsageFlags& flags) const
	{
		auto buffer = m_CommandBuffers[index];

		if (vkResetCommandBuffer(buffer, 0) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error resetting command buffer!");
			return;
		}

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pInheritanceInfo = nullptr;
		beginInfo.flags = flags;
		beginInfo.pNext = nullptr;

		if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error beginning command buffer!");
			return;
		}
	}

	void VulkanCommandPool::EndCommandBuffer(const uint8_t& index) const
	{
		if (vkEndCommandBuffer(m_CommandBuffers[index]) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error beginning command buffer!");
			return;
		}
	}
}
