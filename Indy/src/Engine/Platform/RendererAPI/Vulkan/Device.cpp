#include "Device.h"

#include "Engine/Core/Log.h"

#include <set>

namespace Engine::VulkanAPI
{
	VkDevice Device::s_LogicalDevice;
	VkPhysicalDevice Device::s_PhysicalDevice;
	Queue Device::s_GraphicsQueue;
	Queue Device::s_PresentQueue;

	void Device::Init(const VkInstance& instance, Viewport& viewport)
	{
		Device::ChoosePhysicalDevice(instance, viewport);
		Device::CreateLogicalDevice();
		Device::GenerateQueueHandles();
	}

	void Device::Shutdown()
	{
		vkDestroyDevice(s_LogicalDevice, nullptr);
	}

	void Device::UpdateSurfaceCapabilities(Viewport& viewport)
	{
		// Query for device capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_PhysicalDevice, viewport.surface, &viewport.swapchainSupport.capabilities);

		// Query for format count
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(s_PhysicalDevice, viewport.surface, &formatCount, nullptr);

		// Retrieve formats
		if (formatCount != 0) {
			viewport.swapchainSupport.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(s_PhysicalDevice, viewport.surface, &formatCount, viewport.swapchainSupport.formats.data());
		}

		// Query for presentMode count
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(s_PhysicalDevice, viewport.surface, &presentModeCount, nullptr);

		// Retrieve present modes
		if (presentModeCount != 0) {
			viewport.swapchainSupport.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(s_PhysicalDevice, viewport.surface, &presentModeCount, viewport.swapchainSupport.presentModes.data());
		}
	}

	void Device::UpdateSurfaceCapabilities(Viewport& viewport, VkPhysicalDevice device)
	{
		// Query for device capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, viewport.surface, &viewport.swapchainSupport.capabilities);

		// Query for format count
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, viewport.surface, &formatCount, nullptr);

		// Retrieve formats
		if (formatCount != 0) {
			viewport.swapchainSupport.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, viewport.surface, &formatCount, viewport.swapchainSupport.formats.data());
		}

		// Query for presentMode count
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, viewport.surface, &presentModeCount, nullptr);

		// Retrieve present modes
		if (presentModeCount != 0) {
			viewport.swapchainSupport.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, viewport.surface, &presentModeCount, viewport.swapchainSupport.presentModes.data());
		}
	}

	uint32_t Device::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(s_PhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}

		throw std::runtime_error("[Vulkan Device] Failed to find suitable memory type");

		return 0;
	}

	void Device::ChoosePhysicalDevice(const VkInstance& instance, Viewport& viewport)
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
			if (Device::IsSuitable(viewport, device))
			{
				s_PhysicalDevice = device;
				break;
			}
		}

		if (s_PhysicalDevice == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("[Vulkan Device] Failed to find a suitable GPU!");
		}
	}

	// Physical Device Suitability
	bool Device::IsSuitable(Viewport& viewport, VkPhysicalDevice device)
	{
		bool b_SupportsExtensions = Device::SupportsRequiredExtensions(device);
		bool b_SwapChainAdequate = false;

		if (b_SupportsExtensions)
		{
			Device::UpdateSurfaceCapabilities(viewport, device);
			b_SwapChainAdequate = !viewport.swapchainSupport.formats.empty() && !viewport.swapchainSupport.presentModes.empty();
		}

		return Device::SupportsRequiredQueueFamilies(viewport.surface, device) && b_SupportsExtensions && b_SwapChainAdequate;
	}

	bool Device::SupportsRequiredQueueFamilies(const VkSurfaceKHR& windowSurface, VkPhysicalDevice device)
	{
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
			s_GraphicsQueue.familyIndex = graphicsQueueFamilyIndex.Value();
			s_PresentQueue.familyIndex = presentQueueFamilyIndex.Value();
		}

		// Returns true only if all required queue families are supported.
		return supportsRequiredQueueFamilies;
	}

	bool Device::SupportsRequiredExtensions(VkPhysicalDevice device)
	{
		// Query Device Extension Count
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		// Retrieve Device Extensions
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		// Create a set for the required extensions
		std::set<std::string> requiredExtensions(c_DeviceExtensions.begin(), c_DeviceExtensions.end());

		// Remove extensions that are required, basically marking them as 'supported'
		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		// If any extensions remain, the device does not support all required extensions.
		return requiredExtensions.empty();
	}

	void Device::CreateLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> queueFamilyIndices = {
			s_GraphicsQueue.familyIndex,
			s_PresentQueue.familyIndex
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
		createInfo.enabledExtensionCount = static_cast<uint32_t>(c_DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = c_DeviceExtensions.data();

		if (vkCreateDevice(s_PhysicalDevice, &createInfo, nullptr, &s_LogicalDevice) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}
	}

	void Device::GenerateQueueHandles()
	{
		// Create the Graphics Queue Family Handle
		vkGetDeviceQueue(s_LogicalDevice, s_GraphicsQueue.familyIndex, 0, &s_GraphicsQueue.queue);

		// Create the Presentation Queue Family Handle
		vkGetDeviceQueue(s_LogicalDevice, s_PresentQueue.familyIndex, 0, &s_PresentQueue.queue);
	}
}