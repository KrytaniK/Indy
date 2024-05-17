
#include <vector>
#include <vulkan/vulkan.h>

#include "Engine/Core/LogMacros.h"

import Indy.VulkanRenderer;

namespace Indy
{
	bool QueueFamilyIndices::Complete()
	{
		return graphics.has_value() && present.has_value() && compute.has_value();
	}

	QueueFamilyIndices VulkanQueue::FindQueueFamilies(const VkPhysicalDevice& device, const VulkanDeviceCompatibility& compatibility)
	{
		QueueFamilyIndices indices;

		// Get number of queue families for this device
		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

		// Get all queue families
		std::vector<VkQueueFamilyProperties> queueFamProps(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamProps.data());

		
		int i = 0;
		for (const VkQueueFamilyProperties& props : queueFamProps)
		{
			// TODO: Implement presentation queue support check

			// Graphics Queue
			if (compatibility.graphics && (props.queueFlags & VK_QUEUE_GRAPHICS_BIT))
				indices.graphics = i;

			// Check for presentation queue


			// Compute Queue
			if (compatibility.compute && (props.queueFlags & VK_QUEUE_COMPUTE_BIT))
				indices.compute = i;

			if (indices.Complete())
				break;

			i++;
		}

		return indices;
	}

	VulkanQueue::VulkanQueue(const VkDevice& logicalDevice, const QueueFamilyIndices& queueIndices)
	{
		m_LogicalDevice = logicalDevice;

		if (m_LogicalDevice == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("Failed to create device queues: Invalid logical device");
			return;
		}

		if (queueIndices.graphics.has_value())
		{
			vkGetDeviceQueue(m_LogicalDevice, queueIndices.graphics.value(), 0, &m_Graphics);

			if (m_Graphics == VK_NULL_HANDLE)
			{
				INDY_CORE_ERROR("Failed to create graphics queue!");
				return;
			}
		}

		if (queueIndices.present.has_value())
		{
			vkGetDeviceQueue(m_LogicalDevice, queueIndices.present.value(), 0, &m_Present);

			if (m_Present == VK_NULL_HANDLE)
			{
				INDY_CORE_ERROR("Failed to create presentation queue!");
				return;
			}
		}

		if (queueIndices.compute.has_value() && queueIndices.compute.value() != queueIndices.graphics.value())
		{
			vkGetDeviceQueue(m_LogicalDevice, queueIndices.compute.value(), 0, &m_Compute);

			if (m_Compute == VK_NULL_HANDLE)
			{
				INDY_CORE_ERROR("Failed to create compute queue!");
				return;
			}
		}
	}

}