#pragma once

#include "VulkanDevice.h"

namespace Engine
{

	class VulkanSwapChain
	{
	private:
		static VkSwapchainKHR s_Swapchain;
		static VkExtent2D s_Extent;
		static VkFormat s_ImageFormat;
		static std::vector<VkImage> s_Images;
		static std::vector<VkImageView> s_ImageViews;
		static std::vector<VkFramebuffer> s_Framebuffers;


	public:
		static const VkSwapchainKHR& GetSwapChain() { return s_Swapchain; };
		static const VkExtent2D& GetExtent() { return s_Extent; };
		static const VkFormat& GetImageFormat() { return s_ImageFormat; };
		static const std::vector<VkImage> GetImages() { return s_Images; };
		static const std::vector<VkImageView> GetImageViews() { return s_ImageViews; };
		static std::vector<VkFramebuffer>& GetFrameBuffers() { return s_Framebuffers; };

		static void Init(VkSurfaceKHR windowSurface);
		static void Shutdown();
		static void Cleanup();

		static void CreateImageViews();
		static void CreateFramebuffers();

	private:
		static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		VulkanSwapChain();
	};
}