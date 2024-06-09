#include <Engine/Core/LogMacros.h>

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanCommandPool::~VulkanCommandPool()
	{
		if (m_LogicalDevice == VK_NULL_HANDLE || m_CommandPool == VK_NULL_HANDLE)
			return;

		// Then, destroy the command pool and allocated buffers
		vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
	}

	void VulkanCommandPool::Allocate(const VkDevice& logicalDevice, const uint32_t& queueFamilyIndex, const uint8_t& commandBufferCount)
	{
		
		m_LogicalDevice = logicalDevice;

		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = queueFamilyIndex;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;

		if (vkCreateCommandPool(m_LogicalDevice, &createInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create command pool!");
			return;
		}

		AllocateCommandBuffers(commandBufferCount);

		m_CmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		m_CmdBufferBeginInfo.pInheritanceInfo = nullptr;
		m_CmdBufferBeginInfo.pNext = nullptr;
	}

	void VulkanCommandPool::AllocateCommandBuffers(const uint8_t& commandBufferCount)
	{
		if (m_CommandPool == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("Failed to allocate command buffers. Command pool is null!");
			return;
		}

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
		{
			INDY_CORE_WARN("Attemting to retrieve a command buffer that does not exist!");
			return VK_NULL_HANDLE;
		}

		return m_CommandBuffers[index];
	}

	void VulkanCommandPool::BeginCommandBuffer(const uint8_t& index, const VkCommandBufferUsageFlags& flags)
	{
		m_CmdBufferBeginInfo.flags = flags;

		vkBeginCommandBuffer(m_CommandBuffers[index], &m_CmdBufferBeginInfo);
	}

	void VulkanCommandPool::EndCommandBuffer(const uint8_t& index)
	{
		vkEndCommandBuffer(m_CommandBuffers[index]);
	}
	void VulkanCommandPool::Reset()
	{
		vkResetCommandPool(m_LogicalDevice, m_CommandPool, 0); // Opt for command buffer re-use
	}
}
