#include "SwapChain.h"

#include "Device.h"

#include "Engine/Core/LogMacros.h"

namespace Engine::VulkanAPI
{
	void SwapChain::CreateSwapchain(Viewport& viewport, bool isRefreshed)
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();
		const Queue& graphicsQueue = Device::GetGraphicsQueue();
		const Queue& presentQueue = Device::GetPresentQueue();

		Device::UpdateSurfaceCapabilities(viewport);

		VkSurfaceFormatKHR surfaceFormat = SwapChain::ChooseSurfaceFormat(viewport.swapchainSupport.formats);
		VkPresentModeKHR presentMode = SwapChain::ChoosePresentMode(viewport.swapchainSupport.presentModes);
		VkExtent2D extent = SwapChain::ChooseExtent((GLFWwindow*)viewport.window, viewport.swapchainSupport.capabilities);

		// Recommended to request at least one more image than the minimum, which
		// means that we don't have to wait on the driver to complete internal operations.
		uint32_t imageCount = viewport.swapchainSupport.capabilities.minImageCount + 1;
		
		// Never exceed max image count. 0 is special, meaning no limit.
		if (viewport.swapchainSupport.capabilities.maxImageCount > 0 && imageCount > viewport.swapchainSupport.capabilities.maxImageCount) {
			imageCount = viewport.swapchainSupport.capabilities.maxImageCount;
		}

		// Resize viewport containers
		viewport.swapchainImages.resize(imageCount);
		viewport.swapchainImageViews.resize(imageCount);
		viewport.framebuffers.resize(imageCount);

		// Swap Chain Create Info
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = viewport.surface;

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
		createInfo.preTransform = viewport.swapchainSupport.capabilities.currentTransform;

		// Specify if the alpha channel should be used for blending with other windows.
		// Best to almost always ignore the alpha channel.
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;

		// The color of pixels obscured by other windows are not cared about.
		createInfo.clipped = VK_TRUE;

		if (isRefreshed)
			createInfo.oldSwapchain = viewport.swapchain;
		else
			createInfo.oldSwapchain = nullptr;

		// Finally, create the swap chain.
		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &viewport.swapchain) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[Vulkan Swap Chain] Failed To Create Swap Chain!");
		}

		// Query Swap Chain Image Handles count
		vkGetSwapchainImagesKHR(logicalDevice, viewport.swapchain, &imageCount, nullptr);

		// Retrieve Swap Chain Image Handles
		vkGetSwapchainImagesKHR(logicalDevice, viewport.swapchain, &imageCount, viewport.swapchainImages.data());

		// Store Swap Chain format and extent for later use.
		viewport.imageFormat = surfaceFormat.format;
		viewport.extent = extent;
	}

	void SwapChain::RefreshSwapchain(Viewport& viewport, const VkRenderPass& renderPass)
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();

		vkDeviceWaitIdle(logicalDevice);

		viewport.Cleanup(logicalDevice, false);

		SwapChain::CreateSwapchain(viewport, true);
		SwapChain::CreateImageViews(viewport);
		SwapChain::CreateFramebuffers(viewport, renderPass);
	}

	void SwapChain::CreateImageViews(Viewport& viewport)
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();

		// Loop over all the created swap chain images
		for (size_t i = 0; i < viewport.swapchainImageViews.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = viewport.swapchainImages[i];

			// Specify how images are treated (1D, 2D, 3D Textures, or cube maps)
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = viewport.imageFormat;

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

			if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &viewport.swapchainImageViews[i]) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("[Vulkan Swap Chain] Could not create image views!");
			}
		}
	}

	void SwapChain::CreateFramebuffers(Viewport& viewport, const VkRenderPass& renderPass)
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();

		for (size_t i = 0; i < viewport.swapchainImageViews.size(); i++)
		{
			VkImageView attachments[] = {
				viewport.swapchainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

			framebufferInfo.renderPass = renderPass;

			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = viewport.extent.width;
			framebufferInfo.height = viewport.extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &viewport.framebuffers[i]) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("[VulkanAPI] Failed to create framebuffer!");
			}
		}
	}

	void SwapChain::CreateSyncObjects(Viewport& viewport)
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < viewport.frames.size(); i++)
		{
			if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &viewport.frames[i].imageAvailableSemaphore) != VK_SUCCESS ||
				vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &viewport.frames[i].renderFinishedSemaphore) != VK_SUCCESS ||
				vkCreateFence(logicalDevice, &fenceInfo, nullptr, &viewport.frames[i].fence) != VK_SUCCESS) {
				INDY_CORE_ERROR("failed to create semaphores!");
			}
		}
	}

	VkSurfaceFormatKHR SwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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

	VkPresentModeKHR SwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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

	VkExtent2D SwapChain::ChooseExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
}