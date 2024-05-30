module;

#include <span>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

export module Indy.VulkanGraphics:Image;

import :DescriptorPool;

export
{
	namespace Indy
	{
		struct VulkanImageSpec
		{
			VkFormat format;
			VkExtent3D extent;
			VkImageUsageFlags usageFlags;
			VkImageAspectFlags aspectFlags;
			VkDescriptorSet descriptorSet;
		};

		class VulkanImage
		{
		public:
			static void Copy(const VkCommandBuffer& commandBuffer, const VkImage& src, const VkImage& dst, const VkExtent2D& srcExtent, const VkExtent2D& dstExtent);

			static void TransitionLayout(const VkCommandBuffer& commandBuffer, const VkImage& image, const VkImageLayout& currentLayout, const VkImageLayout& newLayout);

		public:
			VulkanImage(const VmaAllocator& allocator, const VkDevice& logicalDevice, const VulkanImageSpec& spec);
			~VulkanImage();

			const VkExtent3D& GetExtent() { return m_ImageExtent; };
			const VkImage& Get() { return m_Image; };
			const VkImageView& GetView() { return m_ImageView; };

		private:
			const VmaAllocator* m_Allocator;
			VmaAllocation m_Allocation;

			VkDevice m_LogicalDevice;

			VkImage m_Image;
			VkImageView m_ImageView;
			VkExtent3D m_ImageExtent;
			VkFormat m_Format;
		};
	}
}