
#include <vector>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanDescriptor::VulkanDescriptor(VulkanDescriptorPool* descriptorPool, const VkDescriptorSetLayout& layout)
	{
		m_Layout = layout;
		m_Set = descriptorPool->AllocateDescriptorSet(layout);
	}

	void VulkanDescriptor::UpdateBufferBinding(const VkDescriptorType& descriptorType, uint32_t binding, VkDescriptorBufferInfo* bufferInfos, const uint32_t& elementOffset, const uint32_t& updateCount)
	{
		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = nullptr;
		write.dstBinding = binding;
		write.dstSet = m_Set;
		write.descriptorCount = updateCount; // # of descriptors to update
		write.pBufferInfo = bufferInfos;
		write.descriptorType = descriptorType;
		write.dstArrayElement = elementOffset;

		m_Writes.push_back(write);
	}

	void VulkanDescriptor::UpdateImageBinding(const VkDescriptorType& descriptorType, uint32_t binding, VkDescriptorImageInfo* imageInfos, const uint32_t& elementOffset, const uint32_t& updateCount)
	{
		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = nullptr;
		write.dstBinding = binding;
		write.dstSet = m_Set;
		write.descriptorCount = updateCount; // # of descriptors to update
		write.pImageInfo = imageInfos;
		write.descriptorType = descriptorType;
		write.dstArrayElement = elementOffset;

		m_Writes.push_back(write);
	}

	void VulkanDescriptor::UpdateDescriptorSets(const VkDevice& logicalDevice)
	{
		vkUpdateDescriptorSets(logicalDevice, m_Writes.size(), m_Writes.data(), 0, nullptr);
		m_Writes.clear();
	}
}
