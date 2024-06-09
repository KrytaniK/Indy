module;

#include <memory>
#include <vector>
#include <optional>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

export module Indy.VulkanGraphics:Device;

import :Swapchain;

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
		struct VulkanDeviceCompatibility
		{
			GPUCompatLevel graphics = COMPAT_VOID;
			GPUCompatLevel compute = COMPAT_VOID;
			GPUCompatLevel geometryShader = COMPAT_VOID;
			VkPhysicalDeviceType type = VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;
			GPUCompatLevel typePreference = COMPAT_PREFER;
		};

		// Wrapper structure for GPU queue family indices
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics;
			std::optional<uint32_t> present;
			std::optional<uint32_t> compute;

			bool Complete()
			{
				return graphics.has_value() && present.has_value() && compute.has_value();
			};
		};

		// A containing structure for Vulkan Physical Devices and their properties and features.
		struct VulkanPhysicalDevice
		{
			VkPhysicalDevice handle = VK_NULL_HANDLE;
			VkPhysicalDeviceProperties properties{};
			VkPhysicalDeviceFeatures features{};
			QueueFamilyIndices queueFamilies{};
			VulkanSwapchainSupport swapchainSupport{};
		};

		class VulkanAPI;

		// A utility wrapper class representing a Vulkan Logical Device
		class VulkanDevice
		{
		private:
			static std::vector<VulkanPhysicalDevice> s_PhysicalDevices;

		public:
			static void GetAllGPUSpecs(const VkInstance& instance);
			static bool GetGPUSurfaceSupport(VulkanPhysicalDevice& gpu, const VkSurfaceKHR& surface);
			static VulkanPhysicalDevice ChoosePhysicalDevice(const VulkanDeviceCompatibility& compatibility);

		private:
			static bool CheckFeatureCompatibility(const GPUCompatLevel& preference, bool hasFeature, uint8_t& rating);

		public:
			// Creates a logical device based on a physical device, chosen based on some compatibility
			VulkanDevice(const VkInstance& instance, const VulkanDeviceCompatibility& compatibility);

			// Creates a logical device from a physical device
			VulkanDevice(const VkInstance& instance, const VulkanPhysicalDevice& physicalDevice);

			~VulkanDevice();

			const VkDevice& Get();
			const VulkanPhysicalDevice& GetPhysicalDevice();
			const VmaAllocator& GetVmaAllocator() const { return m_Allocator; };

		private:
			void CreateLogicalDevice();

		private:
			VmaAllocator m_Allocator;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			VulkanPhysicalDevice m_PhysicalDevice;
		};
	}
}