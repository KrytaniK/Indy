module;

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Device;

import :Utils;
import :Queue;

export
{
	namespace Indy::Graphics
	{
		// A utility wrapper class representing a Vulkan Logical Device
		class VulkanDevice
		{
		public:
			VulkanDevice(const VulkanDeviceCompatibility& compatibility);
			~VulkanDevice();

		private:
			void CreateLogicalDevice(const VulkanDeviceCompatibility& compatibility);

		private:
			std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			std::unique_ptr<VulkanQueue> m_Queue;
		};
	}
}