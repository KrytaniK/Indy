#include "VulkanSwapChain.h"

#include "VulkanPipeline.h"

#include "Engine/Core/Log.h"
#include "Engine/EventSystem/Events.h"

#include <GLFW\glfw3.h>

namespace Engine
{
	VkSwapchainKHR VulkanSwapChain::s_Swapchain;
	VkExtent2D VulkanSwapChain::s_Extent;
	VkFormat VulkanSwapChain::s_ImageFormat;
	std::vector<VkImage> VulkanSwapChain::s_Images;
	std::vector<VkImageView> VulkanSwapChain::s_ImageViews;
	std::vector<VkFramebuffer> VulkanSwapChain::s_Framebuffers;

	void VulkanSwapChain::Init(VkSurfaceKHR windowSurface)
	{
		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();
		const VulkanQueue& graphicsQueue = VulkanDevice::GetGraphicsQueue();
		const VulkanQueue& presentQueue = VulkanDevice::GetPresentQueue();
		const VulkanSwapChainSupport& swapchainSupport = VulkanDevice::GetSwapChainSupport(windowSurface);

		VkSurfaceFormatKHR surfaceFormat = VulkanSwapChain::ChooseSurfaceFormat(swapchainSupport.formats);
		VkPresentModeKHR presentMode = VulkanSwapChain::ChoosePresentMode(swapchainSupport.presentModes);
		VkExtent2D extent = VulkanSwapChain::ChooseExtent(swapchainSupport.capabilities);
		
		// Recommended to request at least one more image than the minimum, which
		// means that we don't have to wait on the driver to complete internal operations.
		uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
		
		// Never exceed max image count. 0 is special, meaning no limit.
		if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
			imageCount = swapchainSupport.capabilities.maxImageCount;
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
		
		uint32_t queueFamilyIndices[] = { graphicsQueue.familyIndex, presentQueue.familyIndex };
		
		// If the queue families are different, concurrent mode ensures
		// the swap chain images are shared across queue families.
		// Exclusive mode is best for performance.
		if (graphicsQueue.familyIndex != presentQueue.familyIndex) {
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
		createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
		
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
		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &s_Swapchain) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[Vulkan Swap Chain] Failed To Create Swap Chain!");
		}
		
		// Query Swap Chain Image Handles count
		vkGetSwapchainImagesKHR(logicalDevice, s_Swapchain, &imageCount, nullptr);
		
		s_Images.resize(imageCount); // Resize the container
		
		// Retrieve Swap Chain Image Handles
		vkGetSwapchainImagesKHR(logicalDevice, s_Swapchain, &imageCount, s_Images.data());
		
		// Store Swap Chain format and extent for later use.
		s_ImageFormat = surfaceFormat.format;
		s_Extent = extent;
	}

	void VulkanSwapChain::Shutdown()
	{
		Cleanup();
	}

	void VulkanSwapChain::Cleanup()
	{
		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();

		for (auto framebuffer : s_Framebuffers)
		{
			vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		}

		for (size_t i = 0; i < s_ImageViews.size(); i++)
		{
			vkDestroyImageView(logicalDevice, s_ImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(logicalDevice, s_Swapchain, nullptr);
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
		else 
		{
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
		if (s_Swapchain == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("[Vulkan Swap Chain] Cannot Create Image Views: Swap chain has not been initialized!");
			return;
		}

		if (s_Images.empty())
		{
			INDY_CORE_ERROR("[Vulkan Swap Chain] Cannot Create Image Views: Swap Chain Images have not been created!");
			return;
		}

		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();

		// Ensure the number of image views matches the number of images.
		s_ImageViews.resize(s_Images.size());

		// Loop over all the created swap chain images
		for (size_t i = 0; i < s_Images.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = s_Images[i];

			// Specify how images are treated (1D, 2D, 3D Textures, or cube maps)
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = s_ImageFormat;

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

			if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &s_ImageViews[i]) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("[Vulkan Swap Chain] Could not create image views!");
			}
		}
	}
	
	void VulkanSwapChain::CreateFramebuffers()
	{
		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();
		const VkRenderPass& renderPass = VulkanPipeline::GetRenderPass();

		s_Framebuffers.resize(s_ImageViews.size());

		for (size_t i = 0; i < s_ImageViews.size(); i++)
		{
			VkImageView attachments[] = {
				s_ImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

			framebufferInfo.renderPass = renderPass;

			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = s_Extent.width;
			framebufferInfo.height = s_Extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &s_Framebuffers[i]) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("[VulkanAPI] Failed to create framebuffer!");
			}
		}
	}
}