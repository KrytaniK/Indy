#include <Engine/Core/LogMacros.h>

#include <string>
#include <unordered_map>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanSyncObjects::VulkanSyncObjects(const VkDevice& logicalDevice)
	{
		m_LogicalDevice = logicalDevice;
	}

	VulkanSyncObjects::~VulkanSyncObjects()
	{
		for (const auto& fence : m_Fences)
			vkDestroyFence(m_LogicalDevice, fence.second, nullptr);

		for (const auto& semaphore : m_Semaphores)
			vkDestroySemaphore(m_LogicalDevice, semaphore.second, nullptr);
	}

	const VkFence& VulkanSyncObjects::GetFence(const std::string& fence) const
	{
		auto result = m_Fences.find(fence);

		if (result == m_Fences.end())
			return VK_NULL_HANDLE;

		return result->second;
	}

	const VkSemaphore& VulkanSyncObjects::GetSemaphore(const std::string& semaphore) const
	{
		auto result = m_Semaphores.find(semaphore);

		if (result == m_Semaphores.end())
			return VK_NULL_HANDLE;

		return result->second;
	}

	void VulkanSyncObjects::AddFence(const std::string& name, const VkFenceCreateFlags& flags)
	{
		VkFenceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = flags;

		if (vkCreateFence(m_LogicalDevice, &createInfo, nullptr, &m_Fences[name]) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create sync object [VkFence]!");
			return;
		}
	}

	void VulkanSyncObjects::AddSemaphore(const std::string& name, const VkSemaphoreCreateFlags& flags)
	{
		VkSemaphoreCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = flags;

		if (vkCreateSemaphore(m_LogicalDevice, &createInfo, nullptr, &m_Semaphores[name]) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create sync object [VkSemaphore]!");
			return;
		}
	}

}