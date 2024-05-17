module;

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

export module Indy.VulkanRenderer:Device;

import :Utils;
import :Queue;

export
{
	namespace Indy
	{
		// A utility wrapper class representing a Vulkan Logical Device
		class VulkanDevice
		{
		public:
			// A utility method for retrieving all GPUs, as well as their properties and features.
			static std::vector<VulkanPhysicalDevice> GetAllPhysicalDevices(const VkInstance& instance);
			// A utility method for checking basic GPU compatibility.
			static bool IsDeviceCompatible(const VulkanPhysicalDevice& device, const VulkanDeviceCompatibility& targetComp);

		public:
			VulkanDevice(VulkanSharedResources* shared, const VulkanDeviceCompatibility& compatibility);
			~VulkanDevice();

		private:
			void CreateLogicalDevice(const VulkanDeviceCompatibility& compatibility);

		private:
			VulkanSharedResources* m_sharedResources = nullptr;
			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			std::unique_ptr<VulkanQueue> m_Queue;
		};
	}
}