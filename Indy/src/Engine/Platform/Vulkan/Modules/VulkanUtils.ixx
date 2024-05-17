module;

#include <vector>

#include <vulkan/vulkan.h>

export module Indy.VulkanRenderer:Utils;

export
{
	namespace Indy
	{
		// A container structure used for finding a GPU that meets some basic criteria.
		struct VulkanDeviceCompatibility
		{
			bool graphics = false; // Should this GPU support graphics operations?
			bool present = false; // Should this GPU support presenting to a window?
			bool compute = false; // Should this GPU support compute operations?
			bool geometryShader = false; // Should this GPU support geometry shaders?
			VkPhysicalDeviceType deviceType = VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM; // GPU type, one of VkPhysicalDeviceType (e.g., Discrete(Dedicated), Integrated, CPU, etc.)
		};

		// A containing structure for Vulkan Physical Devices and their properties and features.
		struct VulkanPhysicalDevice
		{
			VkPhysicalDevice device = VK_NULL_HANDLE;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;
		};

		// A containing structure for sharing vulkan resources across renderers.
		struct VulkanSharedResources
		{
			VkInstance instance = VK_NULL_HANDLE;
			std::vector<VulkanPhysicalDevice> devices;
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		};
	}
}