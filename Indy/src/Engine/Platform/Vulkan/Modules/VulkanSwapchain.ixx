module;

#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Swapchain;

import :Device;

import Indy.Window;

export
{
	namespace Indy
	{
		struct VulkanSwapchain
		{
			static VulkanSwapchain Create(Window* window, const VkSurfaceKHR& surface, const std::shared_ptr<VulkanDevice>& device);

			VkSwapchainKHR handle = VK_NULL_HANDLE;
			VkExtent2D extent{};
			VkSurfaceFormatKHR format{};
			VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
			std::vector<VkImage> images;
			std::vector<VkImageView> imageViews;
			uint32_t imageIndex = 0;
		};
	}
}