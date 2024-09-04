#include <Engine/Core/LogMacros.h>

#include <vector>

#include <vulkan/vulkan.h>

import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy::Graphics
{
	VulkanQueue::VulkanQueue(const VkPhysicalDevice& physicalDevice)
	{
		// Query for the number of queue families for the chosen device
		uint32_t qfCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &qfCount, nullptr);

		// Retrieve all queue family properties
		std::vector<VkQueueFamilyProperties> qfProps(qfCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &qfCount, qfProps.data());

		// Query each queue index
		for (uint32_t i = 0; i < qfCount; i++)
		{
			const VkQueueFamilyProperties& props = qfProps[i];

			if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				m_GraphicsIndex = i;

			if (props.queueFlags & VK_QUEUE_COMPUTE_BIT)
				m_ComputeIndex = i;
		}
	}

	VulkanQueue::~VulkanQueue()
	{

	}

	const VkQueue& VulkanQueue::GetCompute()
	{
		return m_Compute;
	}

	const std::optional<uint32_t>& VulkanQueue::GetComputeIndex()
	{
		return m_ComputeIndex;
	}

	const VkQueue& VulkanQueue::GetGraphics()
	{
		return m_Graphics;
	}

	const std::optional<uint32_t>& VulkanQueue::GetGraphicsIndex()
	{
		return m_GraphicsIndex;
	}

	void VulkanQueue::Bind(const VkDevice& logicalDevice)
	{
		// Retrieve Queue Handles
		VkDeviceQueueInfo2 queueGetInfo{};
		queueGetInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
		queueGetInfo.pNext = nullptr;
		queueGetInfo.flags = 0;
		queueGetInfo.queueIndex = 0;

		// Retrieve Compute Queue handle
		queueGetInfo.queueFamilyIndex = m_ComputeIndex.value();
		vkGetDeviceQueue2(logicalDevice, &queueGetInfo, &m_Compute);

		// Retrieve Graphics Queue handle
		queueGetInfo.queueFamilyIndex = m_GraphicsIndex.value();
		vkGetDeviceQueue2(logicalDevice, &queueGetInfo, &m_Graphics);
	}

	bool VulkanQueue::HasDedicatedCompute()
	{
		return m_ComputeIndex.has_value() && m_ComputeIndex.value() != m_GraphicsIndex.value();
	}

	bool VulkanQueue::ComputeIsGraphics()
	{
		return m_ComputeIndex.value() == m_GraphicsIndex.value();
	}

}