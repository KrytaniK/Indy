#include <Engine/Core/LogMacros.h>

#include <memory>
#include <set>

#include <vulkan/vulkan.h>

import Indy.VulkanRenderer;

namespace Indy
{
	std::vector<VulkanPhysicalDevice> VulkanDevice::GetAllPhysicalDevices(const VkInstance& instance)
	{
		if (!instance)
		{
			INDY_CORE_ERROR("Could not get any GPUs. Invalid Vulkan instance!");
			return {};
		}

		// Query physical device count
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			INDY_CORE_ERROR("Could not find any GPUs with Vulkan support!");
			return {};
		}

		// Create wrapper device vector and vulkan device vector
		std::vector<VulkanPhysicalDevice> pDevices(deviceCount);
		std::vector<VkPhysicalDevice> devices(deviceCount);

		INDY_CORE_TRACE("Enumerating Physical Devices...");

		// Retrieve all physical devices
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		int i = 0;
		for (const VkPhysicalDevice device : devices)
		{
			// Copy each physical device into the wrapper vector
			pDevices[i].device = device;

			// Copy all device properties and features into the wrapper vector
			vkGetPhysicalDeviceProperties(device, &pDevices[i].properties);
			vkGetPhysicalDeviceFeatures(device, &pDevices[i].features);

			i++;
		}

		// return the wrapper vector
		return pDevices;
	}

	bool VulkanDevice::IsDeviceCompatible(const VulkanPhysicalDevice& physicalDevice, const VulkanDeviceCompatibility& targetComp)
	{
		INDY_CORE_TRACE("Checking device compatibility for [{0}]...", physicalDevice.properties.deviceName);

		bool isSuitable = false;
		QueueFamilyIndices indices = VulkanQueue::FindQueueFamilies(physicalDevice.device, targetComp);

		if (targetComp.deviceType != VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM) // Always match device type, if specified
			isSuitable = targetComp.deviceType == physicalDevice.properties.deviceType;

		if (targetComp.graphics) // Target GPUs that support graphics operations
			isSuitable = indices.graphics.has_value();

		if (targetComp.present) // Target GPUs that support presenting to a VkSurface
			isSuitable = indices.present.has_value();

		if (targetComp.compute) // Target GPUs that support compute operations
			isSuitable = indices.compute.has_value();

		if (targetComp.geometryShader) // Target GPUs that support geometry shaders
			isSuitable = physicalDevice.features.geometryShader;

		INDY_CORE_TRACE("Suitable? {0}...", isSuitable);

		return isSuitable;
	}

	VulkanDevice::VulkanDevice(VulkanSharedResources* shared, const VulkanDeviceCompatibility& compatibility)
		: m_sharedResources(shared)
	{
		INDY_CORE_TRACE("Choosing a physical device to interface with...");

		// Choose a compatible physical device. Always compare against all physical devices
		for (const auto& physicalDevice : m_sharedResources->devices)
		{
			if (VulkanDevice::IsDeviceCompatible(physicalDevice, compatibility))
				m_PhysicalDevice = physicalDevice.device;
		}

		CreateLogicalDevice(compatibility);
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(m_LogicalDevice, nullptr);
	}

	void VulkanDevice::CreateLogicalDevice(const VulkanDeviceCompatibility& compatibility)
	{
		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("Failed to create logical device. No compatible GPU found!");
			return;
		}

		INDY_CORE_TRACE("Creating logical device...");

		// Get required queue family indices
		QueueFamilyIndices indices = VulkanQueue::FindQueueFamilies(m_PhysicalDevice, compatibility);

		// Queue Priorities
		float queuePriority = 1.0f;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> uniqueIndices;

		if (indices.graphics.has_value())
			uniqueIndices.emplace(indices.graphics.value());

		if (indices.present.has_value())
			uniqueIndices.emplace(indices.present.value());

		if (indices.compute.has_value())
			uniqueIndices.emplace(indices.compute.value());

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
		if (vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create the logical device!");
			return;
		}

		// Create necessary queues
		m_Queue = std::make_unique<VulkanQueue>(m_LogicalDevice, indices);
	}

}