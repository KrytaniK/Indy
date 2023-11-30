#pragma once

#include "Util.h"

#include <vulkan/vulkan.h>

namespace Engine::VulkanAPI
{
	class Device
	{
	private:
		static VkDevice s_LogicalDevice;
		static VkPhysicalDevice s_PhysicalDevice;
		static Queue s_GraphicsQueue;
		static Queue s_PresentQueue;

	public:
		static void Init(const VkInstance& instance, Viewport& viewport);
		static void Shutdown();

		static void UpdateSurfaceCapabilities(Viewport& viewport);
		static void UpdateSurfaceCapabilities(Viewport& viewport, VkPhysicalDevice device);

		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		static const VkDevice& GetLogicalDevice() { return s_LogicalDevice; };
		static const VkPhysicalDevice& GetPhysicalDevice() { return s_PhysicalDevice; };
		static const Queue& GetGraphicsQueue() { return s_GraphicsQueue; };
		static const Queue& GetPresentQueue() { return s_PresentQueue; };

	private:
		static void ChoosePhysicalDevice(const VkInstance& instance, Viewport& viewport);
		static void CreateLogicalDevice();
		static void GenerateQueueHandles();
		static bool IsSuitable(Viewport& viewport, VkPhysicalDevice device);
		static bool SupportsRequiredQueueFamilies(const VkSurfaceKHR& windowSurface, VkPhysicalDevice device);
		static bool SupportsRequiredExtensions(VkPhysicalDevice device);
	};
}