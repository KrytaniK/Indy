#include <Engine/Core/LogMacros.h>

#include <memory>
#include <set>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

import Indy.Events;

namespace Indy::Graphics
{
	VulkanDevice::VulkanDevice(const VulkanDeviceCompatibility& compatibility)
	{
		INDY_CORE_TRACE("[Vulkan Device] Choosing a physical device to interface with...");
		// Dispatch an event to get the most compatible physical device
		VulkanGPUEvent event;
		event.compatibility = &compatibility;

		Events<VulkanGPUEvent>::Notify(&event);

		if (event.outDevice)
			m_PhysicalDevice = event.outDevice;

		CreateLogicalDevice(compatibility);
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(m_LogicalDevice, nullptr);
	}

	void VulkanDevice::CreateLogicalDevice(const VulkanDeviceCompatibility& compatibility)
	{
		if (!m_PhysicalDevice)
		{
			INDY_CORE_ERROR("Failed to create logical device. No compatible GPU!");
			return;
		}

		INDY_CORE_TRACE("Creating logical device...");

		// Queue Priorities
		float queuePriority = 1.0f;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> uniqueIndices;

		if (m_PhysicalDevice->queueFamilies.graphics.has_value())
			uniqueIndices.emplace(m_PhysicalDevice->queueFamilies.graphics.value());

		if (m_PhysicalDevice->queueFamilies.present.has_value())
			uniqueIndices.emplace(m_PhysicalDevice->queueFamilies.present.value());

		if (m_PhysicalDevice->queueFamilies.compute.has_value())
			uniqueIndices.emplace(m_PhysicalDevice->queueFamilies.compute.value());

		// Attach each unique queue create info
		for (uint32_t index : uniqueIndices)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = index;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Features specified via compatibility (e.g., geometry shaders, tessellation shaders, etc.)
		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.geometryShader = compatibility.geometryShader;

		// Initialize logical device creation structure
		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

#ifdef ENGINE_DEBUG
		// Validation layer support (specified for legacy versions)
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(g_Vulkan_Validation_Layers.size());
		deviceCreateInfo.ppEnabledLayerNames = g_Vulkan_Validation_Layers.data();
#else
		deviceCreateInfo.enabledLayerCount = 0;
#endif

		// Create the logical device
		if (vkCreateDevice(m_PhysicalDevice->handle, &deviceCreateInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create the logical device!");
			return;
		}

		// Create necessary queues
		m_Queue = std::make_unique<VulkanQueue>(m_LogicalDevice, m_PhysicalDevice->queueFamilies);
	}

}