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
	namespace Indy::Graphics
	{
		typedef enum VKCompatLevel : uint8_t {
			COMPAT_VOID = 0x00,
			COMPAT_PREFER,
			COMPAT_REQUIRED
		} VKCompatLevel;

		// A container structure used for finding a GPU that meets some basic criteria.
		struct VKDeviceCompat
		{
			VKCompatLevel graphics = COMPAT_VOID;
			VKCompatLevel compute = COMPAT_VOID;
			VKCompatLevel geometryShader = COMPAT_VOID;
			VkPhysicalDeviceType type = VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;
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
		public:
			VulkanDevice(const VKDeviceCompat& compatibility);
			VulkanDevice(const VKDeviceCompat& compatibility, const VkSurfaceKHR& surface);
			~VulkanDevice();

			const std::shared_ptr<VulkanPhysicalDevice>& GetPhysicalDevice();
			const VkDevice& Get();

			static std::vector<std::shared_ptr<VulkanPhysicalDevice>> GetAllPhysicalDevices(const VkInstance& instance);

			static std::shared_ptr<VulkanPhysicalDevice> GetCompatibleDevice(const std::vector<std::shared_ptr<VulkanPhysicalDevice>>& devices, const VKDeviceCompat& compatibility);

			static uint8_t RateDeviceCompatibility(const VulkanPhysicalDevice& device, const VKDeviceCompat& compatibility);

			static bool IsCompatibleFeature(const VKCompatLevel& preference, bool hasFeature, uint8_t& rating);

			static bool SupportsPresentation(VulkanPhysicalDevice& device, const VkSurfaceKHR& surface);

		private:
			void CreateLogicalDevice();

		private:
			std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			std::unique_ptr<VulkanQueue> m_Queue;
		};
	}
}