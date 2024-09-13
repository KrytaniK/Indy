module;

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Device;

import :Queue;

export
{
	namespace Indy::Graphics
	{
		struct VulkanDeviceConfig
		{
			VkPhysicalDevice* physicalDevices = nullptr;
			uint32_t physicalDeviceCount = 0;
			
			VkPhysicalDeviceFeatures2 features{}; // Complete Vulkan Device Features
			VkPhysicalDeviceFeatures coreFeatures{}; // Core Vulkan Device features
			VkPhysicalDeviceVulkan12Features features12{}; // Vulkan 1.2 Device Features
			VkPhysicalDeviceVulkan13Features features13{}; // Vulkan 1.3 Device Features

			std::vector<const char*> extensions;
			std::vector<const char*> debugLayers;
		};

		class VulkanDevice
		{
		public:
			VulkanDevice(const VulkanDeviceConfig& config, const VkInstance& instance);
			~VulkanDevice();

			const VkDevice& Get();
			const VkPhysicalDevice& GetPhysicalDevice();
			const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures();
			const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties();

			const VulkanQueue& Queues();

		private:
			VkInstance m_Instance;

			VkDevice m_Device;
			VkPhysicalDevice m_PhysicalDevice;

			VkPhysicalDeviceFeatures m_PDeviceFeatures;
			VkPhysicalDeviceProperties m_PDeviceProperties;

			std::unique_ptr<VulkanQueue> m_Queues;
		};
	}
}