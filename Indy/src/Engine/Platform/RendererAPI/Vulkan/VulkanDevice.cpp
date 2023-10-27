#include "VulkanDevice.h"

#include "Engine/Core/Log.h"

#include <set>

namespace Engine
{
	VulkanDeviceInfo VulkanDevice::s_DeviceInfo;

	void VulkanDevice::Init(VkInstance instance, VkSurfaceKHR windowSurface)
	{
		if (instance == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("[Vulkan Device] Cannot create required devices: Vulkan Instance is null!");
			return;
		}

		if (windowSurface == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("[Vulkan Device] Cannot create required devices: Window Surface is null!");
			return;
		}

		ChoosePhysicalDevice(instance, windowSurface);
		CreateLogicalDevice();
		GenerateQueueHandles();
	}

	void VulkanDevice::Shutdown()
	{
		// Physical Device is automatically cleaned up by Vulkan
		
		vkDestroyDevice(s_DeviceInfo.logicalDevice, nullptr);
	}

	// ---------------
	// Physical Device
	// ---------------

	void VulkanDevice::ChoosePhysicalDevice(VkInstance instance, VkSurfaceKHR windowSurface)
	{
		// Query for physical device count
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			INDY_CORE_ERROR("[Vulkan Device] Failed to find GPUs with Vulkan Support!");
			return;
		}

		// Get all GPUs that support Vulkan
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			// Only choose a GPU suitable for the required commands.
			if (IsSuitable(device, windowSurface))
			{
				s_DeviceInfo.physicalDevice = device;
				break;
			}
		}

		if (s_DeviceInfo.physicalDevice == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("[Vulkan Device] Failed to find a suitable GPU!");
		}
	}

	// Device Suitability
	bool VulkanDevice::IsSuitable(VkPhysicalDevice device, VkSurfaceKHR windowSurface)
	{
		// Todo: Add SwapChain Support
		return SupportsRequiredQueueFamilies(device, windowSurface) && SupportsRequiredExtensions(device) && WindowSurfaceSupportsSwapChain(device, windowSurface);
	}

	// Queue Family Support
	bool VulkanDevice::SupportsRequiredQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR windowSurface)
	{
		if (device == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("[Vulkan] Failed to query for queue families: No valid physical device");
			return false;
		}

		QueueFamilyIndex graphicsQueueFamilyIndex;
		QueueFamilyIndex presentQueueFamilyIndex;

		// Query for the number of queue families on this device
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		// Actually retrieve all queue families
		std::vector<VkQueueFamilyProperties> deviceQueueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, deviceQueueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : deviceQueueFamilies)
		{
			// Query Graphics command support
			bool supportsGraphicsQueue = queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;

			// Query Presentation command support
			VkBool32 supportsPresentQueue = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, windowSurface, &supportsPresentQueue);

			if (supportsGraphicsQueue)
				graphicsQueueFamilyIndex.Set(i);

			if (supportsPresentQueue)
				presentQueueFamilyIndex.Set(i);

			i++;
		}

		bool supportsRequiredQueueFamilies = graphicsQueueFamilyIndex.IsValid() && presentQueueFamilyIndex.IsValid();

		if (supportsRequiredQueueFamilies)
		{
			// Attach logical device and queue family indices
			s_DeviceInfo.graphicsQueueFamilyIndex = graphicsQueueFamilyIndex.Value();
			s_DeviceInfo.presentQueueFamilyIndex = presentQueueFamilyIndex.Value();
		}

		// Returns true only if all required queue families are supported.
		return supportsRequiredQueueFamilies;
	}

	// Extension Support
	bool VulkanDevice::SupportsRequiredExtensions(VkPhysicalDevice device)
	{
		// Query Device Extension Count
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		// Retrieve Device Extensions
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		// Create a set for the required extensions
		std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

		// Remove extensions that are required, basically marking them as 'supported'
		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		// If any extensions remain, the device does not support all required extensions.
		return requiredExtensions.empty();
	}

	bool VulkanDevice::WindowSurfaceSupportsSwapChain(VkPhysicalDevice device, VkSurfaceKHR windowSurface)
	{

		// Query for device capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, windowSurface, &s_DeviceInfo.support.capabilities);

		// Query for format count
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface, &formatCount, nullptr);

		// Retrieve formats
		if (formatCount != 0) {
			s_DeviceInfo.support.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface, &formatCount, s_DeviceInfo.support.formats.data());
		}

		// Query for presentMode count
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface, &presentModeCount, nullptr);

		// Retrieve present modes
		if (presentModeCount != 0) {
			s_DeviceInfo.support.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface, &presentModeCount, s_DeviceInfo.support.presentModes.data());
		}

		return !s_DeviceInfo.support.formats.empty() && !s_DeviceInfo.support.presentModes.empty();
	}

	// --------------
	// Logical Device
	// --------------

	void VulkanDevice::CreateLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> queueFamilyIndices = { 
			s_DeviceInfo.graphicsQueueFamilyIndex,
			s_DeviceInfo.presentQueueFamilyIndex
		};

		// Construct the createInfo for each queue
		float queuePriority = 1.0f;
		for (uint32_t queueFamilyIndex : queueFamilyIndices)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Specify GPU features you want to use.
		VkPhysicalDeviceFeatures deviceFeatures{};

		// Create the Logical Device
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		// Enable Device Extensions for Swap Chain
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

		if (vkCreateDevice(s_DeviceInfo.physicalDevice, &createInfo, nullptr, &s_DeviceInfo.logicalDevice) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}
	}

	// -------------
	// Queue Handles
	// -------------

	void VulkanDevice::GenerateQueueHandles()
	{
		// Create the Graphics Queue Family Handle
		vkGetDeviceQueue(s_DeviceInfo.logicalDevice, s_DeviceInfo.graphicsQueueFamilyIndex, 0, &s_DeviceInfo.graphicsQueue);

		// Create the Presentation Queue Family Handle
		vkGetDeviceQueue(s_DeviceInfo.logicalDevice, s_DeviceInfo.presentQueueFamilyIndex, 0, &s_DeviceInfo.presentQueue);
	}
}