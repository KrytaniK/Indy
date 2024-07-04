#include<Engine/Core/LogMacros.h>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanImage VulkanImage::Create(const VkDevice& logicalDevice, const VmaAllocator& allocator, const VulkanImageSpec& spec)
	{
		VulkanImage outImage{};

		outImage.extent = spec.extent;
		outImage.format = spec.format;

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

		vmaCreateImage(allocator, &imageCreateInfo, &imgAllocInfo, &outImage.image, &outImage.allocation, nullptr);

		// Image View Creation
		VkImageViewCreateInfo viewCreateInfo{};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.pNext = nullptr;

		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.image = outImage.image;
		viewCreateInfo.format = outImage.format;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;
		viewCreateInfo.subresourceRange.aspectMask = spec.aspectFlags;

		vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &outImage.view);

		// Create Sampler
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // outside image bounds just use border color
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.minLod = -1000;
		samplerInfo.maxLod = 1000;
		samplerInfo.maxAnisotropy = 1.0f;

		vkCreateSampler(logicalDevice, &samplerInfo, nullptr, &outImage.sampler);

		return outImage;
	}

	void VulkanImageProcessor::AddLayoutTransition(
		const VkImage& image,
		const VkImageLayout& currentLayout,
		const VkImageLayout& newLayout,
		const VkPipelineStageFlags2& srcStageMask,
		const VkPipelineStageFlags2& dstStageMask,
		const VkAccessFlags2& srcAccessMask,
		const VkAccessFlags2& dstAccessMask
	)
	{
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

		m_ImageBarriers.emplace_back(imageBarrier);
		m_ImageBarrierCount++;
	}

	void VulkanImageProcessor::TransitionLayouts(const VkCommandBuffer& commandBuffer)
	{
		// Dependency struct
		VkDependencyInfo depInfo{};
		depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		depInfo.pNext = nullptr;

		// Attach image barrier
		depInfo.imageMemoryBarrierCount = m_ImageBarrierCount;
		depInfo.pImageMemoryBarriers = m_ImageBarriers.data();

		vkCmdPipelineBarrier2(commandBuffer, &depInfo);
	}

	void VulkanImageProcessor::ClearTransitions()
	{
		m_ImageBarriers.clear();
		m_ImageBarrierCount = 0;
	}

	void VulkanImageProcessor::CopyImage(const VkCommandBuffer& commandBuffer, const VkImage& src, const VkImage& dst, const VkExtent2D& srcExtent, const VkExtent2D& dstExtent)
	{
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
}
