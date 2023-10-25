#pragma once

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

namespace Engine
{
	class VulkanDevice
	{
	public:
		struct SwapChainSupportDetails
		{
			bool isSupported;
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
			VkDevice logicalDevice;
			uint32_t graphicsQueueFamilyIndex;
			uint32_t presentQueueFamilyIndex;
		};

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

		SwapChainSupportDetails GetSwapChainSupportDetails() { return m_SwapChainSupportDetails; };

	private:
		// Chooses the physical device (GPU) to use for the application.
		void ChoosePhysicalDevice(VkInstance instance, VkSurfaceKHR windowSurface);

		bool IsSuitable(VkPhysicalDevice device, VkSurfaceKHR windowSurface);
		bool SupportsRequiredQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR windowSurface);
		bool SupportsRequiredExtensions(VkPhysicalDevice device);
		bool WindowSurfaceSupportsSwapChain(VkPhysicalDevice device, VkSurfaceKHR windowSurface);

		void CreateLogicalDevice();

		void GenerateQueueHandles();

	private:
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_LogicalDevice;

		QueueFamilyIndex m_GraphicsQueueFamilyIndex; // Index for the Graphics Queue Family
		QueueFamilyIndex m_PresentQueueFamilyIndex; // Index for the Presentation Queue Family

		VkQueue m_GraphicsQueue; // Responsible for graphics commands, such as draw calls.
		VkQueue m_PresentQueue; // Responsible for presenting the window surface to the window.

		SwapChainSupportDetails m_SwapChainSupportDetails;

		const std::vector<const char*> m_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};
	};
}