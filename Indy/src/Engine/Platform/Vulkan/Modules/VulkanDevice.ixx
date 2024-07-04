module;

#include <memory>
#include <vector>
#include <optional>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

export module Indy.VulkanGraphics:Device;

export
{
	namespace Indy
	{
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

		struct VulkanSwapchainSupport
		{
			VkSurfaceCapabilitiesKHR capabilities{};
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		struct VulkanDeviceRequirements
		{
			VkPhysicalDeviceFeatures2 features{};
			uint32_t extensionCount;
			const char* const* extensions;
			uint32_t layerCount;
			const char* const* layers;
		};

		struct VulkanDevice
		{
			// Creates a logical device with a physical device that best matches the device requirements.
			static std::shared_ptr<VulkanDevice> Create(const VkInstance& instance, const VulkanDeviceRequirements& reqs);

			VulkanDeviceRequirements deviceRequirements{};
			VkDevice handle = VK_NULL_HANDLE;
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			VkPhysicalDeviceProperties properties{};
			VkPhysicalDeviceFeatures features{};
			QueueFamilyIndices queueFamilies{};
			VulkanSwapchainSupport swapchainSupport{};
			bool querySwapchainSupport;
		};

		// Retrieves all physical devices with Vulkan support
		std::vector<VkPhysicalDevice> GetAllVulkanDevices(const VkInstance& instance);

		// Ensures the physical device can support presenting to a surface. 
		// If supported, recreates the logical device and Vma Allocator handles
		// and updates the queue families and swapchain support members of the VulkanDevice.
		bool QueryVulkanSwapchainSupport(const std::shared_ptr<VulkanDevice>& device, const VkSurfaceKHR& surface);
	}
}