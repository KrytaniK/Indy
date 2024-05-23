#include <Engine/Core/LogMacros.h>

#include <memory>
#include <vector>
#include <set>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

import Indy.Events;

namespace Indy::Graphics
{
	VulkanDevice::VulkanDevice(const VKDeviceCompat& compatibility)
	{
		INDY_CORE_TRACE("[Vulkan Device] Choosing a physical device to interface with...");

		// Get a compatible physical device
		VKDeviceSelectEvent event;
		event.compatCriteria = &compatibility;

		Events<VKDeviceSelectEvent>::Notify(&event);

		m_PhysicalDevice = event.outDevice;

		if (!m_PhysicalDevice)
		{
			INDY_CORE_ERROR("Failed to create logical device. No compatible GPU!");
			return;
		}

		CreateLogicalDevice();
	}

	VulkanDevice::VulkanDevice(const VKDeviceCompat& compatibility, const VkSurfaceKHR& surface)
	{
		INDY_CORE_TRACE("[Vulkan Device] Choosing a physical device to interface with...");

		// Get a compatible physical device
		VKDeviceSelectEvent event;
		event.compatCriteria = &compatibility;

		Events<VKDeviceSelectEvent>::Notify(&event);

		m_PhysicalDevice = event.outDevice;

		if (!m_PhysicalDevice)
		{
			INDY_CORE_ERROR("Failed to create logical device. No compatible GPU!");
			return;
		}

		if (!SupportsPresentation(*m_PhysicalDevice, surface))
		{
			INDY_CORE_ERROR("Failed to create logical device. Does not support surface presentation!");
			return;
		}

		CreateLogicalDevice();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(m_LogicalDevice, nullptr);
	}

	const std::shared_ptr<VulkanPhysicalDevice>& VulkanDevice::GetPhysicalDevice()
	{
		return m_PhysicalDevice;
	}

	const VkDevice& VulkanDevice::Get()
	{
		return m_LogicalDevice;
	}

	void VulkanDevice::CreateLogicalDevice()
	{
		INDY_CORE_TRACE("Creating logical device...");

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

	// -----------------------------------------------------------------------------------------------------------------------
	// -------------------------------------------------- Static Methods -----------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------

	std::vector<std::shared_ptr<VulkanPhysicalDevice>> VulkanDevice::GetAllPhysicalDevices(const VkInstance& instance)
	{
		// Query physical device count
		uint32_t deviceCount = 0;
		if (vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to enumerate physical devices...");
			return {};
		}

		if (deviceCount == 0)
		{
			INDY_CORE_ERROR("Could not find any GPUs with Vulkan support!");
			return {};
		}

		// Create wrapper device vector and vulkan device vector
		std::vector<std::shared_ptr<VulkanPhysicalDevice>> pDevices(deviceCount);
		std::vector<VkPhysicalDevice> devices(deviceCount);

		INDY_CORE_TRACE("Enumerating Physical Devices...");

		// Retrieve all physical devices
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		INDY_CORE_TRACE("Physical Device Count: {0}", deviceCount);

		int i = 0;
		for (VkPhysicalDevice device : devices)
		{
			pDevices[i] = std::make_unique<VulkanPhysicalDevice>();

			// Copy each physical device into the wrapper vector
			pDevices[i]->handle = device;

			// Copy all device properties and features into the wrapper vector
			vkGetPhysicalDeviceProperties(device, &pDevices[i]->properties);
			vkGetPhysicalDeviceFeatures(device, &pDevices[i]->features);

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
					pDevices[i]->queueFamilies.graphics = j;

				// Querying for presentation support happens when it is needed.

				// Check for support with compute operations
				if (props.queueFlags & VK_QUEUE_COMPUTE_BIT)
					pDevices[i]->queueFamilies.compute = j;

				if (pDevices[i]->queueFamilies.Complete())
					break;

				j++;
			}

			INDY_CORE_INFO(
				"\n\t[{0}]\tID: {1}\tType: {2}\n\t\tSupports Graphics: {3}\n\t\tSupports Present: {4}\n\t\tSupports Compute: {5}\n\t\t",
				pDevices[i]->properties.deviceName,
				pDevices[i]->properties.deviceID,
				pDevices[i]->properties.deviceType,
				pDevices[i]->queueFamilies.graphics.has_value(),
				pDevices[i]->queueFamilies.present.has_value(),
				pDevices[i]->queueFamilies.compute.has_value()
			);

			i++;
		}

		// return the wrapper vector
		return pDevices;
	}

	std::shared_ptr<VulkanPhysicalDevice> VulkanDevice::GetCompatibleDevice(const std::vector<std::shared_ptr<VulkanPhysicalDevice>>& devices, const VKDeviceCompat& compatibility)
	{
		std::shared_ptr<VulkanPhysicalDevice> outDevice = nullptr;

		uint8_t highestRating = 0;
		for (auto& device : devices)
		{
			uint8_t rating = RateDeviceCompatibility(*device, compatibility);
			if (rating >= highestRating)
			{
				outDevice = device;
				highestRating = rating;
			}
		}

		return outDevice;
	}

	uint8_t VulkanDevice::RateDeviceCompatibility(const VulkanPhysicalDevice& device, const VKDeviceCompat& compatibility)
	{

		uint8_t rating = 0;

		if (compatibility.type != VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM)
		{
			if (compatibility.type != device.properties.deviceType)
				return 0;

			rating += 10;
		}

		if (!IsCompatibleFeature(
			compatibility.graphics,
			device.queueFamilies.graphics.has_value(),
			rating)
			) return 0;

		if (!IsCompatibleFeature(
			compatibility.compute,
			device.queueFamilies.compute.has_value(),
			rating)
			) return 0;

		if (!IsCompatibleFeature(
			compatibility.geometryShader,
			device.features.geometryShader,
			rating)
			) return 0;


		INDY_CORE_TRACE("Device compatibility rating for [{0}]: {1}.", device.properties.deviceName, rating);
		return rating;
	}

	bool VulkanDevice::IsCompatibleFeature(const VKCompatLevel& preference, bool hasFeature, uint8_t& rating)
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
				return false;
		}
		}

		return true;
	}

	bool VulkanDevice::SupportsPresentation(VulkanPhysicalDevice& device, const VkSurfaceKHR& surface)
	{
		// Query for supported queue families
		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device.handle, &familyCount, nullptr);

		// Get all queue families
		std::vector<VkQueueFamilyProperties> queueFamProps(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device.handle, &familyCount, queueFamProps.data());

		// Query queue family support for each device
		int i = 0;
		for (const VkQueueFamilyProperties& props : queueFamProps)
		{
			// Check for presentation support
			VkBool32 supported = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device.handle, i++, surface, &supported);

			if (supported)
			{
				device.queueFamilies.present = i;
				return true;
			}
		}

		return false;
	}
}
