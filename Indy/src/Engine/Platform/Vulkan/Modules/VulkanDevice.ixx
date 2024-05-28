module;

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Device;

import :Queue;
import :Swapchain;
import :Events;

export
{
	namespace Indy
	{
		typedef enum GPUCompatLevel : uint8_t {
			COMPAT_VOID = 0x00,
			COMPAT_PREFER,
			COMPAT_REQUIRED
		} GPUCompatLevel;

		// A container structure used for finding a GPU that meets some basic criteria.
		struct GPUCompatibility
		{
			GPUCompatLevel graphics = COMPAT_VOID;
			GPUCompatLevel compute = COMPAT_VOID;
			GPUCompatLevel geometryShader = COMPAT_VOID;
			VkPhysicalDeviceType type = VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;
			GPUCompatLevel typePreference = COMPAT_PREFER;
		};

		// A containing structure for Vulkan Physical Devices and their properties and features.
		struct VulkanPhysicalDevice
		{
			VkPhysicalDevice handle = VK_NULL_HANDLE;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;
			QueueFamilyIndices queueFamilies;
			VKSwapchainSupport swapchainSupport;
		};

		class VulkanAPI;

		// A utility wrapper class representing a Vulkan Logical Device
		class VulkanDevice
		{
		private:
			static std::vector<std::shared_ptr<VulkanPhysicalDevice>> s_PhysicalDevices;

		public:
			static void GetAllGPUSpecs(const VkInstance& instance);
			static bool GetGPUSurfaceSupport(const std::shared_ptr<VulkanPhysicalDevice>& gpu, const VkSurfaceKHR& surface);

		public:
			VulkanDevice(const GPUCompatibility& compatibility);
			VulkanDevice(const GPUCompatibility& compatibility, const VkSurfaceKHR& surface);
			~VulkanDevice();

			const std::shared_ptr<VulkanPhysicalDevice>& GetPhysicalDevice();
			const VkDevice& Get();

		private:
			void CreateLogicalDevice();
			void FindCompatibleGPU(const GPUCompatibility& compatibility);
			bool IsCompatibleFeature(const GPUCompatLevel& preference, bool hasFeature, uint8_t& rating);

		private:
			std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			std::unique_ptr<VulkanQueue> m_Queue;
		};
	}
}