module;

#include <vector>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

export module Indy.VulkanGraphics:Image;

export
{
	namespace Indy
	{
		struct VulkanImageSpec
		{
			VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkFormat format;
			VkExtent3D extent;
			VkImageUsageFlags usageFlags;
			VkImageAspectFlags aspectFlags;
			VkDescriptorSet descriptorSet;
		};

		class VulkanImageProcessor
		{
		public:
			VulkanImageProcessor() : m_ImageBarrierCount(0) {};
			~VulkanImageProcessor() = default;

			void AddLayoutTransition(
				const VkImage& image,
				const VkImageLayout& currentLayout,
				const VkImageLayout& newLayout,
				const VkPipelineStageFlags2& srcStageMask,
				const VkPipelineStageFlags2& dstStageMask,
				const VkAccessFlags2& srcAccessMask = 0,
				const VkAccessFlags2& dstAccessMask = 0
			);

			void TransitionLayouts(const VkCommandBuffer& commandBuffer);

			void ClearTransitions();

			void CopyImage(
				const VkCommandBuffer& commandBuffer,
				const VkImage& src,
				const VkImage& dst,
				const VkExtent2D& srcExtent,
				const VkExtent2D& dstExtent
			);

		private:
			std::vector<VkImageMemoryBarrier2> m_ImageBarriers;
			uint32_t m_ImageBarrierCount;
		};

		struct VulkanImage
		{
		public:
			static VulkanImage Create(
				const VkDevice& logicalDevice,
				const VmaAllocator& allocator,
				const VulkanImageSpec& spec
			);

			VmaAllocation allocation = VK_NULL_HANDLE;
			VkSampler sampler = VK_NULL_HANDLE;
			VkImage image = VK_NULL_HANDLE;
			VkImageView view = VK_NULL_HANDLE;
			VkExtent3D extent{};
			VkFormat format{};
		};
	}
}