#include "VulkanDevice.h"

#include "Engine/Core/Log.h"

#include <set>

namespace Engine
{
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
		
		vkDestroyDevice(m_LogicalDevice, nullptr);
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
				m_PhysicalDevice = device;
				break;
			}
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
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
				m_GraphicsQueueFamilyIndex.Set(i);

			if (supportsPresentQueue)
				m_PresentQueueFamilyIndex.Set(i);

			i++;
		}

		// Returns true only if all required queue families are supported.
		return m_GraphicsQueueFamilyIndex.IsValid() && m_PresentQueueFamilyIndex.IsValid();
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
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, windowSurface, &m_SwapChainSupportDetails.capabilities);

		// Query for format count
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface, &formatCount, nullptr);

		// Retrieve formats
		if (formatCount != 0) {
			m_SwapChainSupportDetails.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface, &formatCount, m_SwapChainSupportDetails.formats.data());
		}

		// Query for presentMode count
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface, &presentModeCount, nullptr);

		// Retrieve present modes
		if (presentModeCount != 0) {
			m_SwapChainSupportDetails.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface, &presentModeCount, m_SwapChainSupportDetails.presentModes.data());
		}

		// Attach logical device and queue family indices
		m_SwapChainSupportDetails.graphicsQueueFamilyIndex = m_GraphicsQueueFamilyIndex.Value();
		m_SwapChainSupportDetails.presentQueueFamilyIndex = m_PresentQueueFamilyIndex.Value();

		return !m_SwapChainSupportDetails.formats.empty() && !m_SwapChainSupportDetails.presentModes.empty();
	}

	// --------------
	// Logical Device
	// --------------

	void VulkanDevice::CreateLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> queueFamilyIndices = { 
			m_GraphicsQueueFamilyIndex.Value(),
			m_PresentQueueFamilyIndex.Value() 
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

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		// Additionally store logical device for swap chain support.
		m_SwapChainSupportDetails.logicalDevice = m_LogicalDevice;
	}

	// -------------
	// Queue Handles
	// -------------

	void VulkanDevice::GenerateQueueHandles()
	{
		// Create the Graphics Queue Family Handle
		vkGetDeviceQueue(m_LogicalDevice, m_GraphicsQueueFamilyIndex.Value(), 0, &m_GraphicsQueue);

		// Create the Presentation Queue Family Handle
		vkGetDeviceQueue(m_LogicalDevice, m_PresentQueueFamilyIndex.Value(), 0, &m_PresentQueue);
	}
}