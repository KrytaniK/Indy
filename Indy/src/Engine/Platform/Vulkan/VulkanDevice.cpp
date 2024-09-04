#include <Engine/Core/LogMacros.h>

#include <vector>
#include <memory>
#include <set>

#include <vulkan/vulkan.h>

import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy::Graphics
{
	VulkanDevice::VulkanDevice(const VulkanDeviceConfig& config, const VkInstance& instance)
		: m_Instance(instance), m_Device(VK_NULL_HANDLE), m_PhysicalDevice(VK_NULL_HANDLE), m_PDeviceFeatures({}), m_PDeviceProperties({})
	{
		if (config.physicalDevices == nullptr)
		{
			INDY_CORE_CRITICAL("Failed to create Vulkan Logical Device! Invalid GPU list!");
			return;
		}

		// Physical Device Suitability Checks
		uint32_t bestChoice = 0; // Always default to the first GPU
		uint32_t bestRating = 0;
		for (uint32_t i = 0; i < config.physicalDeviceCount; i++)
		{
			const VkPhysicalDevice& pDevice = config.physicalDevices[i];
			uint32_t suitability = 0;

			VkPhysicalDeviceProperties deviceProps{};
			VkPhysicalDeviceFeatures deviceFeatures{};

			vkGetPhysicalDeviceProperties(pDevice, &deviceProps);
			vkGetPhysicalDeviceFeatures(pDevice, &deviceFeatures);

			// Prefer dedicated graphics cards
			if (deviceProps.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				suitability++;

			// Prefer geometry shaders
			if (deviceFeatures.geometryShader)
				suitability++;

			// Prefer tessellation shaders
			if (deviceFeatures.tessellationShader)
				suitability++;

			// Check against best choice
			if (suitability > bestRating)
			{
				bestRating = suitability;
				bestChoice = i;
			}

			i++;
		}
		
		// Store Best-Suited Physical Device and its Properties/Features
		m_PhysicalDevice = config.physicalDevices[bestChoice];
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PDeviceProperties);
		vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_PDeviceFeatures);

		// Initialize Device Queues (Compute and Graphics ONLY!)
		m_Queues = std::make_unique<VulkanQueue>(m_PhysicalDevice);

		// Create the logical device
		float queuePriority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueIndices;

		if (m_Queues->GetGraphicsIndex().has_value())
			uniqueIndices.emplace(m_Queues->GetGraphicsIndex().value());

		if (m_Queues->GetComputeIndex().has_value())
			uniqueIndices.emplace(m_Queues->GetComputeIndex().value());

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

		// Initialize logical device creation structure
		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

		// Enable required extensions
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(config.extensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = config.extensions.data();

#ifdef ENGINE_DEBUG
		// Validation layer support (specified for legacy versions)
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(config.debugLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = config.debugLayers.data();
#else
		deviceCreateInfo.enabledLayerCount = 0;
#endif

		deviceCreateInfo.pNext = &config.features;

		// Create the logical device
		if (vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create the logical device!");
			return;
		}

		// Bind compute and graphics queues to this device
		m_Queues->Bind(m_Device);
	}

	VulkanDevice::~VulkanDevice()
	{

	}

	const VkDevice& VulkanDevice::Get()
	{
		return m_Device;
	}

	const VkPhysicalDevice& VulkanDevice::GetPhysicalDevice()
	{
		return m_PhysicalDevice;
	}

	const VkPhysicalDeviceFeatures& VulkanDevice::GetPhysicalDeviceFeatures()
	{
		return m_PDeviceFeatures;
	}

	const VkPhysicalDeviceProperties& VulkanDevice::GetPhysicalDeviceProperties()
	{
		return m_PDeviceProperties;
	}

	const VulkanQueue& VulkanDevice::Queues()
	{
		return *m_Queues;
	}
}