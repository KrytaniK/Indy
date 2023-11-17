#pragma once

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

namespace Engine
{
	struct VulkanQueue
	{
		VkQueue queue;
		uint32_t familyIndex;
	};

	struct VulkanSwapChainSupport
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanDevice
	{
	private:
		struct QueueFamilyIndex
		{
			std::optional<uint32_t> index;

			bool IsValid() { return index.has_value(); };
			uint32_t Value() { return index.value(); };
			void Set(uint32_t value) { index = value; };
		};

	private:
		static std::vector<const char*> s_DeviceExtensions;

		static VkDevice s_LogicalDevice;
		static VkPhysicalDevice s_PhysicalDevice;
		static VulkanQueue s_GraphicsQueue;
		static VulkanQueue s_PresentQueue;
		static VulkanSwapChainSupport s_SwapChainSupport;

	public:
		static const VkDevice& GetLogicalDevice() { return s_LogicalDevice; };
		static const VkPhysicalDevice& GetPhysicalDevice() { return s_PhysicalDevice; };
		static const VulkanQueue& GetGraphicsQueue() { return s_GraphicsQueue; };
		static const VulkanQueue& GetPresentQueue() { return s_PresentQueue; };
		static const VulkanSwapChainSupport& GetSwapChainSupport(VkSurfaceKHR windowSurface) { 
			VulkanDevice::QuerySwapChainSupport(s_PhysicalDevice, windowSurface);
			return s_SwapChainSupport; 
		};

		static void Init(VkInstance, VkSurfaceKHR windowSurface);
		static void Shutdown();

		static bool QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR windowSurface);

	private:
		static void ChoosePhysicalDevice(VkInstance instance, VkSurfaceKHR windowSurface);
		static void CreateLogicalDevice();
		static void GenerateQueueHandles();
		static bool IsSuitable(VkPhysicalDevice device, VkSurfaceKHR windowSurface);
		static bool SupportsRequiredQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR windowSurface);
		static bool SupportsRequiredExtensions(VkPhysicalDevice device);
	};
}