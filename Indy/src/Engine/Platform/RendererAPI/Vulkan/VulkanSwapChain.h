#pragma once

#include "VulkanDevice.h"

namespace Engine
{
	struct VulkanSwapchainInfo
	{
		VkSwapchainKHR swapchain;
		VkExtent2D extent;
		VkFormat imageFormat;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
	};

	class VulkanSwapChain
	{
	private:
		static VulkanSwapchainInfo s_SwapchainInfo;

	public:
		static const VulkanSwapchainInfo& GetSwapChainInfo() { return s_SwapchainInfo; };

	public:
		void Init(VkSurfaceKHR windowSurface);

		void Shutdown();

		void CreateImageViews();

	private:
		VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	};
}