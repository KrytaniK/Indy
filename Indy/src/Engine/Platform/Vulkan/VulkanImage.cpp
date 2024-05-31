#include<Engine/Core/LogMacros.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

import Indy.VulkanGraphics;

namespace Indy
{
	void VulkanImage::Copy(const VkCommandBuffer& commandBuffer, const VkImage& src, const VkImage& dst,
		const VkExtent2D& srcExtent, const VkExtent2D& dstExtent)
	{
		// Blit images are slower, but are more flexible with how you can copy.

		VkImageBlit2 blitRegion{};
		blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
		blitRegion.pNext = nullptr;

		// Source size
		blitRegion.srcOffsets[1].x = srcExtent.width;
		blitRegion.srcOffsets[1].y = srcExtent.height;
		blitRegion.srcOffsets[1].z = 1;

		// Destination size
		blitRegion.dstOffsets[1].x = dstExtent.width;
		blitRegion.dstOffsets[1].y = dstExtent.height;
		blitRegion.dstOffsets[1].z = 1;

		// Source subresource description
		blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blitRegion.srcSubresource.baseArrayLayer = 0;
		blitRegion.srcSubresource.layerCount = 1;
		blitRegion.srcSubresource.mipLevel = 0;

		// Destination subresource description
		blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blitRegion.dstSubresource.baseArrayLayer = 0;
		blitRegion.dstSubresource.layerCount = 1;
		blitRegion.dstSubresource.mipLevel = 0;

		// Blit Command Info
		VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
		blitInfo.dstImage = dst;
		blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		blitInfo.srcImage = src;
		blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		blitInfo.filter = VK_FILTER_LINEAR;
		blitInfo.regionCount = 1;
		blitInfo.pRegions = &blitRegion;

		vkCmdBlitImage2(commandBuffer, &blitInfo);
	}

	void VulkanImage::TransitionLayout(
		const VkCommandBuffer& commandBuffer, 
		const VkImage& image,
		const VkImageLayout& currentLayout, 
		const VkImageLayout& newLayout,
		const VkPipelineStageFlags2& srcStageMask,
		const VkPipelineStageFlags2& dstStageMask,
		const VkAccessFlags2& srcAccessMask,
		const VkAccessFlags2& dstAccessMask
	)
	{
		// NOTE: This is inefficient and binds the pipeline for a bit. Look into improving this

		VkImageMemoryBarrier2 imageBarrier{};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		imageBarrier.pNext = nullptr;

		// Set barrier masks
		imageBarrier.srcStageMask = srcStageMask;
		imageBarrier.srcAccessMask = srcAccessMask;
		imageBarrier.dstStageMask = dstStageMask;
		imageBarrier.dstAccessMask = dstAccessMask;

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
		imageBarrier.image = image;

		// Dependency struct
		VkDependencyInfo depInfo{};
		depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		depInfo.pNext = nullptr;

		// Attach image barrier
		depInfo.imageMemoryBarrierCount = 1;
		depInfo.pImageMemoryBarriers = &imageBarrier;

		vkCmdPipelineBarrier2(commandBuffer, &depInfo);
	}

	VulkanImage::VulkanImage(const VmaAllocator& allocator, const VkDevice& logicalDevice, const VulkanImageSpec& spec)
	{
		m_Allocator = &allocator;
		m_LogicalDevice = logicalDevice;
		m_ImageExtent = spec.extent;
		m_Format = spec.format;

		// Image Creation
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;

		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.initialLayout = spec.initialLayout;

		imageCreateInfo.format = spec.format;
		imageCreateInfo.extent = spec.extent;

		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;

		// For MSAA
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		// Optimal tiling
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = spec.usageFlags;

		// Allocate image from gpu local memory
		VmaAllocationCreateInfo imgAllocInfo{};
		imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		imgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vmaCreateImage(allocator, &imageCreateInfo, &imgAllocInfo, &m_Image, &m_Allocation, nullptr) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create Image!");
			return;
		}

		// Image View Creation
		VkImageViewCreateInfo viewCreateInfo{};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.pNext = nullptr;
		
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.image = m_Image;
		viewCreateInfo.format = spec.format;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;
		viewCreateInfo.subresourceRange.aspectMask = spec.aspectFlags;

		if (vkCreateImageView(m_LogicalDevice, &viewCreateInfo, nullptr, &m_ImageView) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create Image View!");
			return;
		}
	}

	VulkanImage::~VulkanImage()
	{
		vkDestroyImageView(m_LogicalDevice, m_ImageView, nullptr);
		vmaDestroyImage(*m_Allocator, m_Image, m_Allocation);
	}
}
