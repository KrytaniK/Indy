#include "VulkanSwapChain.h"

#include "Engine/Core/Log.h"
#include "Engine/EventSystem/Events.h"

#include <GLFW\glfw3.h>

namespace Engine
{
	VulkanSwapchainInfo VulkanSwapChain::s_SwapchainInfo;

	void VulkanSwapChain::Init(VkSurfaceKHR windowSurface)
	{
		const VulkanDeviceInfo& deviceInfo = VulkanDevice::GetDeviceInfo();

		VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(deviceInfo.support.formats);
		VkPresentModeKHR presentMode = ChoosePresentMode(deviceInfo.support.presentModes);
		VkExtent2D extent = ChooseExtent(deviceInfo.support.capabilities);
		
		// Recommended to request at least one more image than the minimum, which
		// means that we don't have to wait on the driver to complete internal operations.
		uint32_t imageCount = deviceInfo.support.capabilities.minImageCount + 1;
		
		// Never exceed max image count. 0 is special, meaning no limit.
		if (deviceInfo.support.capabilities.maxImageCount > 0 && imageCount > deviceInfo.support.capabilities.maxImageCount) {
			imageCount = deviceInfo.support.capabilities.maxImageCount;
		}
		
		// Swap Chain Create Info
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = windowSurface;
		
		// Swap Chain Image Details
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		
		uint32_t queueFamilyIndices[] = { deviceInfo.graphicsQueueFamilyIndex, deviceInfo.presentQueueFamilyIndex };
		
		// If the queue families are different, concurrent mode ensures
		// the swap chain images are shared across queue families.
		// Exclusive mode is best for performance.
		if (deviceInfo.graphicsQueueFamilyIndex != deviceInfo.presentQueueFamilyIndex) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}
		
		// Swap chain image transformations can be specified, if supported. Setting
		// this to the current transform means that no transformations are applied.
		createInfo.preTransform = deviceInfo.support.capabilities.currentTransform;
		
		// Specify if the alpha channel should be used for blending with other windows.
		// Best to almost always ignore the alpha channel.
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		
		createInfo.presentMode = presentMode;
		
		// The color of pixels obscured by other windows are not cared about.
		createInfo.clipped = VK_TRUE;
		
		// Definitely worth noting that swap chains are not absolute. They can
		// become invalid or unoptimized while the app is running, which means
		// that an entirely new swap chain must be created from scratch. The
		// reference to the old swap chain must be specified in this field.
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		
		// Finally, create the swap chain.
		if (vkCreateSwapchainKHR(deviceInfo.logicalDevice, &createInfo, nullptr, &s_SwapchainInfo.swapchain) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[Vulkan Swap Chain] Failed To Create Swap Chain!");
		}
		
		// Query Swap Chain Image Handles count
		vkGetSwapchainImagesKHR(deviceInfo.logicalDevice, s_SwapchainInfo.swapchain, &imageCount, nullptr);
		
		s_SwapchainInfo.images.resize(imageCount); // Resize the container
		
		// Retrieve Swap Chain Image Handles
		vkGetSwapchainImagesKHR(deviceInfo.logicalDevice, s_SwapchainInfo.swapchain, &imageCount, s_SwapchainInfo.images.data());
		
		// Store Swap Chain format and extent for later use.
		s_SwapchainInfo.imageFormat = surfaceFormat.format;
		s_SwapchainInfo.extent = extent;


	}

	void VulkanSwapChain::Shutdown()
	{
		const VulkanDeviceInfo& deviceInfo = VulkanDevice::GetDeviceInfo();

		vkDestroySwapchainKHR(deviceInfo.logicalDevice, s_SwapchainInfo.swapchain, nullptr);
	}

	VkSurfaceFormatKHR VulkanSwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		// Check for preferred color format (BGRA and SRGB in this case)
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		// Some additional code could be added to rank formats and return the
		// best one, but that's not really needed right now.
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		// Vulkan has 4 different present modes. Mailbox is equivelant to "triple 
		// buffered", whereas Fifo is regular v-sync.
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		// Get Window
		Event event{ "LayerContext", "RequestWindow" };
		Events::Dispatch(event);
		GLFWwindow* GLFW_Window = (GLFWwindow*)event.data;

		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(GLFW_Window, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	void VulkanSwapChain::CreateImageViews()
	{
		if (s_SwapchainInfo.swapchain == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("[Vulkan Swap Chain] Cannot Create Image Views: Swap chain has not been initialized!");
			return;
		}

		if (s_SwapchainInfo.images.empty())
		{
			INDY_CORE_ERROR("[Vulkan Swap Chain] Cannot Create Image Views: Swap Chain Images have not been created!");
			return;
		}

		// Ensure the number of image views matches the number of images.
		s_SwapchainInfo.imageViews.resize(s_SwapchainInfo.images.size());

		// Loop over all the created swap chain images
		for (size_t i = 0; i < s_SwapchainInfo.images.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = s_SwapchainInfo.images[i];

			// Specify how images are treated (1D, 2D, 3D Textures, or cube maps)
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = s_SwapchainInfo.imageFormat;

			// Default color mapping
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			// Specify the image's purpose and which part should be accessed.
			// This specifies the use of the images as color targets without
			//	mipmapping levels or multiple layers.
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			const VulkanDeviceInfo& deviceInfo = VulkanDevice::GetDeviceInfo();
			if (vkCreateImageView(deviceInfo.logicalDevice, &createInfo, nullptr, &s_SwapchainInfo.imageViews[i]) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("[Vulkan Swap Chain] Could not create image views!");
			}
		}
	}
}