module;

#include <vector>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Descriptor;

import :DescriptorPool;

export
{
	namespace Indy
	{
		class VulkanDescriptor
		{
		public:
			VulkanDescriptor() = default;
			VulkanDescriptor(const VulkanDescriptorPool& descriptorPool, const VkDescriptorSetLayout& layout);

			void UpdateBufferBinding(const VkDescriptorType& descriptorType, uint32_t binding, VkDescriptorBufferInfo* bufferInfos, const uint32_t& elementOffset = 0, const uint32_t& updateCount = 1);
			void UpdateImageBinding(const VkDescriptorType& descriptorType, uint32_t binding, VkDescriptorImageInfo* imageInfos, const uint32_t& elementOffset = 0, const uint32_t& updateCount = 1);

			void UpdateDescriptorSets(const VkDevice& logicalDevice);

			const VkDescriptorSet& GetSet() { return m_Set; };
			const VkDescriptorSetLayout& GetLayout() { return m_Layout; };

		private:
			VkDescriptorSetLayout m_Layout;
			VkDescriptorSet m_Set;
			std::vector<VkWriteDescriptorSet> m_Writes;
		};
	}
}