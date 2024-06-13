#include <Engine/Core/LogMacros.h>

#include <memory>
#include <vector>
#include <set>

#include <vulkan/vulkan.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

import Indy.VulkanGraphics;

import Indy.Events;

/* Creating a VmaAllocator

	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.physicalDevice = physicalDevice;
	allocatorInfo.device = logicalDevice;
	allocatorInfo.instance = instance;
	allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT; // or other flags

	vmaCreateAllocator(&allocatorInfo, &m_Allocator);

*/

namespace Indy
{
	std::shared_ptr<VulkanDevice> VulkanDevice::Create(const VkInstance& instance, const VulkanDeviceRequirements& reqs)
	{
		std::shared_ptr<VulkanDevice> device = std::make_shared<VulkanDevice>();

		// Choose a suitable physical device
		{
			std::vector<VkPhysicalDevice> physicalDevices = GetAllVulkanDevices(instance);

			uint32_t i = 0, bestChoice = 0, bestRating = 0;
			for (const auto& physicalDevice : physicalDevices)
			{
				uint32_t suitabilityRating = 0;

				QueueFamilyIndices queueFamilyIndices{};
				VkPhysicalDeviceProperties deviceProps{};
				VkPhysicalDeviceFeatures deviceFeatures{};

				vkGetPhysicalDeviceProperties(physicalDevice, &deviceProps);
				vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

				// Base suitability checks

				// Prefer dedicated graphics cards
				if (deviceProps.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
					suitabilityRating++;

				// Prefer geometry shaders
				if (deviceFeatures.geometryShader)
					suitabilityRating++;

				// Prefer tessellation shaders
				if (deviceFeatures.tessellationShader)
					suitabilityRating++;

				// Check against best choice
				if (suitabilityRating > bestRating)
				{
					bestRating = suitabilityRating;
					bestChoice = i;
				}

				i++;
			}

			// Query for the device features and properties of the best choice, and attach
			// them to the output device.
			device->physicalDevice = physicalDevices[bestChoice];
			vkGetPhysicalDeviceProperties(device->physicalDevice, &device->properties);
			vkGetPhysicalDeviceFeatures(device->physicalDevice, &device->features);

			// Query for supported queue families
			uint32_t familyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device->physicalDevice, &familyCount, nullptr);

			// Get all queue families
			std::vector<VkQueueFamilyProperties> queueFamProps(familyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device->physicalDevice, &familyCount, queueFamProps.data());

			// Query queue family support for the chosen physical device
			uint32_t j = 0;
			for (const VkQueueFamilyProperties& props : queueFamProps)
			{
				// Check for support with graphics operations
				if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					device->queueFamilies.graphics = j;

				// Check for support with compute operations
				if (props.queueFlags & VK_QUEUE_COMPUTE_BIT)
					device->queueFamilies.compute = j;

				j++;
			}
		}

		// Create the logical device handle
		{
			float queuePriority = 1.0f;
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueIndices;

			if (device->queueFamilies.graphics.has_value())
				uniqueIndices.emplace(device->queueFamilies.graphics.value());

			if (device->queueFamilies.compute.has_value())
				uniqueIndices.emplace(device->queueFamilies.compute.value());

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
			deviceCreateInfo.enabledExtensionCount = reqs.extensionCount;
			deviceCreateInfo.ppEnabledExtensionNames = reqs.extensions;

#ifdef ENGINE_DEBUG
			// Validation layer support (specified for legacy versions)
			deviceCreateInfo.enabledLayerCount = reqs.layerCount;
			deviceCreateInfo.ppEnabledLayerNames = reqs.layers;
#else
			deviceCreateInfo.enabledLayerCount = 0;
#endif

			deviceCreateInfo.pNext = &reqs.features;

			// Create the logical device
			if (vkCreateDevice(device->physicalDevice, &deviceCreateInfo, nullptr, &device->handle) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to create the logical device!");
				return nullptr;
			}
		}

		return device;
	}

	std::vector<VkPhysicalDevice> GetAllVulkanDevices(const VkInstance& instance)
	{
		std::vector<VkPhysicalDevice> devices;

		// Query physical device count
		uint32_t deviceCount = 0;
		if (vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to enumerate physical devices...");
			return devices;
		}

		// Ensure we have valid devices
		if (deviceCount == 0)
		{
			INDY_CORE_ERROR("Could not find any GPUs with Vulkan support!");
			return devices;
		}

		// Resize container
		devices.resize(deviceCount);

		// Retrieve all physical devices
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		return devices;
	}

	bool QueryVulkanSwapchainSupport(const std::shared_ptr<VulkanDevice>& device, const VkSurfaceKHR& surface)
	{
		if (device->queueFamilies.present.has_value())
			return true;

		// Find a valid present queue
		VkBool32 supported = VK_FALSE;
		{
			// Query for supported queue families
			uint32_t familyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device->physicalDevice, &familyCount, nullptr);

			// Get all queue families
			std::vector<VkQueueFamilyProperties> queueFamProps(familyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device->physicalDevice, &familyCount, queueFamProps.data());

			// Query queue family support for each device
			for (size_t i = 0; i < queueFamProps.size(); i++)
			{
				// Check for presentation support on this queue
				vkGetPhysicalDeviceSurfaceSupportKHR(device->physicalDevice, static_cast<uint32_t>(i), surface, &supported);

				// keep checking if this queue doesn't support presentation
				if (!supported)
					continue;

				// Break if we've found a valid queue
				device->queueFamilies.present = static_cast<uint32_t>(i);
				break;
			}
		}

		// Bail if a queue wasn't found that is capable of presenting
		if (!supported)
			return false;

		// Query Swapchain Support
		{
			// Get surface capabilities
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physicalDevice, surface, &device->swapchainSupport.capabilities);

			// Get surface formats
			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice, surface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				device->swapchainSupport.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(
					device->physicalDevice,
					surface,
					&formatCount,
					device->swapchainSupport.formats.data()
				);
			}

			// Get present modes
			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice, surface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				device->swapchainSupport.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(
					device->physicalDevice,
					surface,
					&presentModeCount,
					device->swapchainSupport.presentModes.data()
				);
			}
		}

		// For presentation to be supported, we must have formats and present modes.
		return !device->swapchainSupport.formats.empty() && !device->swapchainSupport.presentModes.empty();
	}
}
