#include "Engine/Core/LogMacros.h"

#include <vector>
#include <memory>
#include <algorithm>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


import Indy.Window;
import Indy.VulkanGraphics;

namespace Indy
{
	VulkanSwapchain VulkanSwapchain::Create(Window* window, const VkSurfaceKHR& surface, const std::shared_ptr<VulkanDevice>& device)
	{
		VulkanSwapchain outSwapchain{};

		if (device->physicalDevice == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("Failed to create swap chain! Null Physical Device Reference");
			return {};
		}

		// Choose Swapchain Format
		{
			// Default to the first available format
			outSwapchain.format = device->swapchainSupport.formats[0];

			// Attempt to find the "best-suited" format
			for (const auto& format : device->swapchainSupport.formats)
			{
				if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace	== VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					outSwapchain.format = format;
			}
		}

		// Choose Present Mode
		{
			// Default to V-Sync Enabled (No chance of visual tearing)
			outSwapchain.presentMode = VK_PRESENT_MODE_FIFO_KHR;

			// If available, choose VK_PRESENT_MODE_MAILBOX_KHR for best performance
			//	with little to no visual tearing
			for (const auto& mode : device->swapchainSupport.presentModes) 
			{
				if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
					outSwapchain.presentMode = mode;
			}
		}

		// Choose Extent
		{
			if (device->swapchainSupport.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
				outSwapchain.extent = device->swapchainSupport.capabilities.currentExtent;
			else
			{
				// GLFW works with both pixels and screen coordinates. Vulkan works with pixels
				// Because of this, the pixel resolution may be larger than the resolution in screen coordinates,
				//	so we need to ensure the resolutions match.

				int width, height;
				glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->NativeWindow()), &width, &height);

				outSwapchain.extent = {
					std::clamp(static_cast<uint32_t>(width),
						device->swapchainSupport.capabilities.minImageExtent.width, device->swapchainSupport.capabilities.maxImageExtent.width
					),
					std::clamp(static_cast<uint32_t>(height),
						device->swapchainSupport.capabilities.minImageExtent.height, device->swapchainSupport.capabilities.maxImageExtent.height
					)
				};
			}
		}

		// Create Swapchain
		{
			// Assign the number of images we'd like to have in the swap chain, always 1 more than the minimum to avoid waiting for driver operations,
			//	and never more than the maximum.
			uint32_t imageCount = device->swapchainSupport.capabilities.minImageCount + 1;

			if (device->swapchainSupport.capabilities.maxImageCount > 0 && imageCount > device->swapchainSupport.capabilities.maxImageCount)
				imageCount = device->swapchainSupport.capabilities.maxImageCount;

			VkSwapchainCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.surface = surface;
			createInfo.minImageCount = imageCount;
			createInfo.imageFormat = outSwapchain.format.format;
			createInfo.imageColorSpace = outSwapchain.format.colorSpace;
			createInfo.imageExtent = outSwapchain.extent;
			createInfo.imageArrayLayers = 1;

			// For use with dynamic rendering
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

			uint32_t queueFamilyIndices[] = { device->queueFamilies.graphics.value(), device->queueFamilies.present.value() };
			if (device->queueFamilies.graphics.value() != device->queueFamilies.present.value())
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // images are shared among queue families
				createInfo.queueFamilyIndexCount = 2;
				createInfo.pQueueFamilyIndices = queueFamilyIndices;
			}
			else
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // images are exclusive to queue families. Ownership must be transferred.
				createInfo.queueFamilyIndexCount = 0;
				createInfo.pQueueFamilyIndices = nullptr;
			}

			// Used to alter image transformation before presentation
			createInfo.preTransform = device->swapchainSupport.capabilities.currentTransform;

			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = outSwapchain.presentMode;
			createInfo.clipped = VK_TRUE; // clips pixels covered by another window

			createInfo.oldSwapchain = VK_NULL_HANDLE;

			if (vkCreateSwapchainKHR(device->handle, &createInfo, nullptr, &outSwapchain.handle) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to create swap chain!");
				return{};
			}
		}

		// Create Images and Image Views
		{
			uint32_t imageCount;

			// Retrieve image count from swapchain
			vkGetSwapchainImagesKHR(device->handle, outSwapchain.handle, &imageCount, nullptr);

			// resize vector to match
			outSwapchain.images.resize(imageCount);
			outSwapchain.imageViews.resize(imageCount);

			// retrieve images
			vkGetSwapchainImagesKHR(device->handle, outSwapchain.handle, &imageCount, outSwapchain.images.data());

			// Set up image view creation. NOTE: This is for VkSurface image views ONLY.
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = outSwapchain.format.format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			// Create image views
			for (uint32_t i = 0; i < imageCount; i++)
			{
				createInfo.image = outSwapchain.images[i]; // apply current image to createInfo

				// Generate image view
				if (vkCreateImageView(device->handle, &createInfo, nullptr, &outSwapchain.imageViews[i]) != VK_SUCCESS)
				{
					INDY_CORE_ERROR("Failed to create image view!");
					return {};
				}
			}
		}

		return outSwapchain;
	}
}
