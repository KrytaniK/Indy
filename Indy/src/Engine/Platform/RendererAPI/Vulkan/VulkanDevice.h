#pragma once

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

namespace Engine
{
	struct VulkanDeviceInfo
	{
		struct Support
		{
			bool supportsSwapChain;
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		Support support;
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
		uint32_t graphicsQueueFamilyIndex;
		uint32_t presentQueueFamilyIndex;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
	};

	class VulkanDevice
	{
	private:
		static VulkanDeviceInfo s_DeviceInfo;

	public:
		static const VulkanDeviceInfo& GetDeviceInfo() { return s_DeviceInfo; };

	private:
		struct QueueFamilyIndex
		{
			std::optional<uint32_t> index;

			bool IsValid() { return index.has_value(); };
			uint32_t Value() { return index.value(); };
			void Set(uint32_t value) { index = value; };
		};

	public:
		void Init(VkInstance instance, VkSurfaceKHR windowSurface);

		void Shutdown();

	private:
		void ChoosePhysicalDevice(VkInstance instance, VkSurfaceKHR windowSurface);

		bool IsSuitable(VkPhysicalDevice device, VkSurfaceKHR windowSurface);

		bool SupportsRequiredQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR windowSurface);

		bool SupportsRequiredExtensions(VkPhysicalDevice device);

		bool WindowSurfaceSupportsSwapChain(VkPhysicalDevice device, VkSurfaceKHR windowSurface);

		void CreateLogicalDevice();

		void GenerateQueueHandles();

	private:
		const std::vector<const char*> m_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};
	};
}