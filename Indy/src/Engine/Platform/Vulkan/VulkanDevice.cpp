#include <Engine/Core/LogMacros.h>

#include <memory>
#include <vector>
#include <set>

#include <vulkan/vulkan.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

import Indy.VulkanGraphics;

import Indy.Events;

namespace Indy
{
	std::vector<VulkanPhysicalDevice> VulkanDevice::s_PhysicalDevices;

	VulkanDevice::VulkanDevice(const VkInstance& instance, const VulkanDeviceCompatibility& compatibility)
	{
		// Choose a suitable physical device
		m_PhysicalDevice = VulkanDevice::ChoosePhysicalDevice(compatibility);

		// Create Logical Device
		CreateLogicalDevice();

		// Create a memory allocator
		VmaAllocatorCreateInfo allocatorInfo{};
		allocatorInfo.physicalDevice = m_PhysicalDevice.handle;
		allocatorInfo.device = m_LogicalDevice;
		allocatorInfo.instance = instance;
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

		if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create device memory allocator!");
			return;
		}
	}

	VulkanDevice::VulkanDevice(const VkInstance& instance, const VulkanPhysicalDevice& physicalDevice)
	{
		// Copy physical device data
		m_PhysicalDevice = physicalDevice;

		// Create the logical device to interface with the physical device
		CreateLogicalDevice();
		
		// Create the Vulkan Memory Allocator for this device
		VmaAllocatorCreateInfo allocatorInfo{};
		allocatorInfo.physicalDevice = m_PhysicalDevice.handle;
		allocatorInfo.device = m_LogicalDevice;
		allocatorInfo.instance = instance;
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

		if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create device memory allocator!");
			return;
		}

	}

	VulkanDevice::~VulkanDevice()
	{
		vmaDestroyAllocator(m_Allocator);

		// Always destroy logical device LAST
		vkDestroyDevice(m_LogicalDevice, nullptr);
	}

	const VulkanPhysicalDevice& VulkanDevice::GetPhysicalDevice()
	{
		return m_PhysicalDevice;
	}

	const VkDevice& VulkanDevice::Get()
	{
		return m_LogicalDevice;
	}

	void VulkanDevice::CreateLogicalDevice()
	{
		float queuePriority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueIndices;

		if (m_PhysicalDevice.queueFamilies.graphics.has_value())
			uniqueIndices.emplace(m_PhysicalDevice.queueFamilies.graphics.value());

		if (m_PhysicalDevice.queueFamilies.present.has_value())
			uniqueIndices.emplace(m_PhysicalDevice.queueFamilies.present.value());

		if (m_PhysicalDevice.queueFamilies.compute.has_value())
			uniqueIndices.emplace(m_PhysicalDevice.queueFamilies.compute.value());

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
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

		// Enable required extensions
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(g_Vulkan_Device_Extensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = g_Vulkan_Device_Extensions.data();

#ifdef ENGINE_DEBUG
		// Validation layer support (specified for legacy versions)
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(g_Vulkan_Validation_Layers.size());
		deviceCreateInfo.ppEnabledLayerNames = g_Vulkan_Validation_Layers.data();
#else
		deviceCreateInfo.enabledLayerCount = 0;
#endif

		// Core Vulkan Features
		VkPhysicalDeviceFeatures features{};

		// Vulkan 1.2 features
		VkPhysicalDeviceVulkan12Features features12{};
		features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		features12.bufferDeviceAddress = VK_TRUE;
		features12.descriptorIndexing = VK_TRUE;

		// Vulkan 1.3 features
		VkPhysicalDeviceVulkan13Features features13{};
		features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		features13.dynamicRendering = VK_TRUE;
		features13.synchronization2 = VK_TRUE;
		features13.pNext = &features12;

		VkPhysicalDeviceFeatures2 deviceFeatures2{};
		deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceFeatures2.features = features;
		deviceFeatures2.pNext = &features13;

		deviceCreateInfo.pNext = &deviceFeatures2;

		// Create the logical device
		if (vkCreateDevice(m_PhysicalDevice.handle, &deviceCreateInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create the logical device!");
			return;
		}
	}

	// -----------------------------------------------------------------------------------------------------------------------
	// -------------------------------------------------- Static Methods -----------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------

	void VulkanDevice::GetAllGPUSpecs(const VkInstance& instance)
	{
		// Query physical device count
		uint32_t deviceCount = 0;
		if (vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to enumerate physical devices...");
			return;
		}

		if (deviceCount == 0)
		{
			INDY_CORE_ERROR("Could not find any GPUs with Vulkan support!");
			return;
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);

		// Retrieve all physical devices
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// Copy device properties and features into static list
		int i = 0;
		for (VkPhysicalDevice device : devices)
		{
			VulkanDevice::s_PhysicalDevices.emplace_back(VulkanPhysicalDevice());
			auto& static_device = VulkanDevice::s_PhysicalDevices[i];

			// Copy each physical device into the wrapper vector
			static_device.handle = device;

			// Copy all device properties and features into the wrapper vector
			vkGetPhysicalDeviceProperties(device, &static_device.properties);
			vkGetPhysicalDeviceFeatures(device, &static_device.features);

			// Query for supported queue families
			uint32_t familyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

			// Get all queue families
			std::vector<VkQueueFamilyProperties> queueFamProps(familyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamProps.data());

			// Query queue family support for each device
			int j = 0;
			for (const VkQueueFamilyProperties& props : queueFamProps)
			{
				// Check for support with graphics operations
				if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					static_device.queueFamilies.graphics = j;

				// Check for support with compute operations
				if (props.queueFlags & VK_QUEUE_COMPUTE_BIT)
					static_device.queueFamilies.compute = j;

				if (static_device.queueFamilies.Complete())
					break;

				j++;
			}

			i++;
		}
	}

	bool VulkanDevice::GetGPUSurfaceSupport(VulkanPhysicalDevice& gpu,
		const VkSurfaceKHR& surface)
	{
		// Device supports presentation if a present queue has already been found.
		if (gpu.queueFamilies.present.has_value())
			return true;

		// Query for supported queue families
		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(gpu.handle, &familyCount, nullptr);

		// Get all queue families
		std::vector<VkQueueFamilyProperties> queueFamProps(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(gpu.handle, &familyCount, queueFamProps.data());

		// Query queue family support for each device
		VkBool32 supported = VK_FALSE;
		for (size_t i = 0; i < queueFamProps.size(); i++)
		{
			// Check for presentation support
			vkGetPhysicalDeviceSurfaceSupportKHR(gpu.handle, static_cast<uint32_t>(i), surface, &supported);

			if (!supported)
				continue;

			gpu.queueFamilies.present = static_cast<uint32_t>(i);
			break;
		}

		if (!supported) // Early return if a queue wasn't found
			return false;

		// If a surface is supported, we need to ensure the swap chain is supported on this device.
		// While this is implied, it's good to check anyway.

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(gpu.handle, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(gpu.handle, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(g_Vulkan_Device_Extensions.begin(), g_Vulkan_Device_Extensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		supported = requiredExtensions.empty();

		if (supported)
		{
			VulkanSwapchain::QuerySupportDetails(gpu, surface);

			// For presentation to be supported, we must have formats and present modes.
			supported = !gpu.swapchainSupport.formats.empty() && !gpu.swapchainSupport.presentModes.empty();
		}

		return supported;
	}

	VulkanPhysicalDevice VulkanDevice::ChoosePhysicalDevice(const VulkanDeviceCompatibility& compatibility)
	{
		size_t outIndex = 0;

		uint8_t highestRating = 0;
		for (size_t i = 0; i < VulkanDevice::s_PhysicalDevices.size(); i++)
		{
			auto& device = VulkanDevice::s_PhysicalDevices[i];

			if (device.handle == VK_NULL_HANDLE)
				continue;

			// Rate device compatibility
			uint8_t rating = 0;

			if (!VulkanDevice::CheckFeatureCompatibility(
				compatibility.typePreference,
				compatibility.type == device.properties.deviceType,
				rating
			)) continue;

			if (!VulkanDevice::CheckFeatureCompatibility(
				compatibility.graphics,
				device.queueFamilies.graphics.has_value(),
				rating
			)) continue;

			if (!VulkanDevice::CheckFeatureCompatibility(
				compatibility.compute,
				device.queueFamilies.compute.has_value(),
				rating
			)) continue;

			if (!VulkanDevice::CheckFeatureCompatibility(
				compatibility.geometryShader,
				device.features.geometryShader,
				rating
			)) continue;

			if (rating >= highestRating)
			{
				outIndex = i;
				highestRating = rating;
			}
		}

		return VulkanDevice::s_PhysicalDevices[outIndex];
	}

	bool VulkanDevice::CheckFeatureCompatibility(const GPUCompatLevel& preference, bool hasFeature, uint8_t& rating)
	{
		switch (preference)
		{
		case COMPAT_VOID: return true;
		case COMPAT_PREFER:
		{
			if (hasFeature)
				rating += 10;
		};
		case COMPAT_REQUIRED:
		{
			if (hasFeature)
				rating += 10;
			else
			{
				rating = 0;
				return false;
			}
		}
		}

		return true;
	}

}
