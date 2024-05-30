module;

#include <span>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:DescriptorPool;

export
{
	namespace Indy
	{
		class VulkanDescriptorPool
		{
		public:
			struct Ratio
			{
				VkDescriptorType type;
				float ratio;
			};

		public:
			VulkanDescriptorPool(const VkDevice& logicalDevice, const uint32_t& maxSets, std::span<Ratio>);
			~VulkanDescriptorPool();

			VkDescriptorSet AllocateDescriptorSet(const VkDescriptorSetLayout& layout);

			void ClearDescriptors();

		private:
			VkDevice m_LogicalDevice;
			VkDescriptorPool m_DescriptorPool;
		};
	}
}