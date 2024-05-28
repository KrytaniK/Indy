#include "Engine/Core/LogMacros.h"

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
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