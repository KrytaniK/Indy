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
	VulkanSwapchain::VulkanSwapchain(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface, Window* window)
	{
		if (!physicalDevice)
		{
			INDY_CORE_ERROR("Failed to create swap chain! Null Physical Device Reference");
			return;
		}

		m_LogicalDevice = logicalDevice;

		// Choose the best format
		ChooseSurfaceFormat(physicalDevice->swapchainSupport.formats);

		// Choose the best present mode
		ChoosePresentMode(physicalDevice->swapchainSupport.presentModes);

		// Choose the best extent
		ChooseExtent(physicalDevice->swapchainSupport.capabilities, static_cast<GLFWwindow*>(window->NativeWindow()));

		// Assign the number of images we'd like to have in the swap chain, always 1 more than the minimum to avoid waiting for driver operations,
		//	and never more than the maximum.
		uint32_t imageCount = physicalDevice->swapchainSupport.capabilities.minImageCount + 1;
		if (physicalDevice->swapchainSupport.capabilities.maxImageCount > 0 && 
			imageCount > physicalDevice->swapchainSupport.capabilities.maxImageCount) {
			imageCount = physicalDevice->swapchainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_Format.format;
		createInfo.imageColorSpace = m_Format.colorSpace;
		createInfo.imageExtent = m_Extent;
		createInfo.imageArrayLayers = 1;

		// For use with dynamic rendering
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		uint32_t queueFamilyIndices[] = { physicalDevice->queueFamilies.graphics.value(), physicalDevice->queueFamilies.present.value() };
		if (physicalDevice->queueFamilies.graphics.value() != physicalDevice->queueFamilies.present.value())
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
		createInfo.preTransform = physicalDevice->swapchainSupport.capabilities.currentTransform;

		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_PresentMode;
		createInfo.clipped = VK_TRUE; // clips pixels covered by another window

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create swap chain!");
			return;
		}

		GetSwapchainImages();
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		for (auto& image : m_Images)
		{
			vkDestroyImageView(m_LogicalDevice, image.imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_LogicalDevice, m_Swapchain, nullptr);
	}

	void VulkanSwapchain::TransitionImage(const VkCommandBuffer& commandBuffer, const uint32_t& imageIndex,
		const VkImageLayout& currentLayout, const VkImageLayout& newLayout)
	{
		// NOTE: This is inefficient and binds the pipeline for a bit. Look into improving this

		VkImageMemoryBarrier2 imageBarrier{};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		imageBarrier.pNext = nullptr;

		// Set barrier masks
		imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
		imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

		// Transition from old to new layout
		imageBarrier.oldLayout = currentLayout;
		imageBarrier.newLayout = newLayout;

		// Create Image Subresource Range with aspect mask
		VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		VkImageSubresourceRange subImage{};
		subImage.aspectMask = aspectMask;
		subImage.baseMipLevel = 0;
		subImage.levelCount = VK_REMAINING_MIP_LEVELS;
		subImage.baseArrayLayer = 0;
		subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

		// Create aspect mask
		imageBarrier.subresourceRange = subImage;
		imageBarrier.image = m_Images[imageIndex].image;

		// Dependency struct
		VkDependencyInfo depInfo{};
		depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		depInfo.pNext = nullptr;

		// Attach image barrier
		depInfo.imageMemoryBarrierCount = 1;
		depInfo.pImageMemoryBarriers = &imageBarrier;

		vkCmdPipelineBarrier2(commandBuffer, &depInfo);
	}

	void VulkanSwapchain::ClearImage(const VkCommandBuffer& commandBuffer, const uint8_t& frameNumber, const uint32_t& imageIndex, const VkImageLayout& imageLayout)
	{
		// Create clear color from frame number
		VkClearColorValue clearColorValue{};
		clearColorValue = {{0.0f, 0.0f, std::abs(std::sin(frameNumber / 120.f))}};

		// Specify subresource range
		VkImageSubresourceRange clearRange{};
		clearRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		clearRange.baseMipLevel = 0;
		clearRange.levelCount = VK_REMAINING_MIP_LEVELS;
		clearRange.baseArrayLayer = 0;
		clearRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

		// Clear image
		vkCmdClearColorImage(commandBuffer, m_Images[imageIndex].image, imageLayout, &clearColorValue, 1, &clearRange);
	}

	void VulkanSwapchain::ChooseSurfaceFormat(const ::std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& format : availableFormats)
		{
			// Always prefer 'SRGB'
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				m_Format = format;
				return;
			}
		}

		// Otherwise return the first available format. It will likely be "good enough".
		m_Format = availableFormats[0];
	}

	void VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& mode : availablePresentModes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) { // Prefer triple-buffering (swap frames for newest version)
				m_PresentMode = mode;
				return;
			}
		}

		m_PresentMode = VK_PRESENT_MODE_FIFO_KHR; // otherwise opt for frame queue
	}

	void VulkanSwapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			m_Extent = capabilities.currentExtent;
			return;
		}

		// GLFW works with both pixels and screen coordinates. Vulkan works with pixels
		// Because of this, the pixel resolution may be larger than the resolution in screen coordinates,
		//	so we need to ensure the resolutions match.

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		m_Extent = {
			std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
	}

	void VulkanSwapchain::GetSwapchainImages()
	{
		uint32_t imageCount;
		std::vector<VkImage> images;

		// Retrieve image count from swapchain
		vkGetSwapchainImagesKHR(m_LogicalDevice, m_Swapchain, &imageCount, nullptr);

		// resize vectors to match
		images.resize(imageCount);
		m_Images.resize(imageCount);

		// retrieve images
		vkGetSwapchainImagesKHR(m_LogicalDevice, m_Swapchain, &imageCount, images.data());

		// Set up image view creation. NOTE: This is for VkSurface image views ONLY.
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_Format.format;
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
			m_Images[i].image = images[i]; // copy image into private member

			createInfo.image = images[i]; // apply current image to createInfo

			// Generate image view
			if (vkCreateImageView(m_LogicalDevice, &createInfo, nullptr, &m_Images[i].imageView) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to create image view!");
				return;
			}
		}
	}

	// -----------------------------------------------------------------------------------------------------------------------
	// -------------------------------------------------- Static Methods -----------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------

	void VulkanSwapchain::QuerySupportDetails(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice, const VkSurfaceKHR& surface)
	{
		if (!physicalDevice)
		{
			INDY_CORE_ERROR("Failed to get swapchain support details! Null Physical Device Reference");
			return;
		}

		// Get surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->handle, surface, &physicalDevice->swapchainSupport.capabilities);

		// Get surface formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->handle, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			physicalDevice->swapchainSupport.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(
				physicalDevice->handle, 
				surface, 
				&formatCount, 
				physicalDevice->swapchainSupport.formats.data()
			);
		}

		// Get present modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->handle, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			physicalDevice->swapchainSupport.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				physicalDevice->handle, 
				surface, 
				&presentModeCount, 
				physicalDevice->swapchainSupport.presentModes.data()
			);
		}
	}
}
