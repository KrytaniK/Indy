#include <Engine/Core/LogMacros.h>

#include <vector>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	// ------------------------
	// SET LAYOUT BUILDER -----
	// ------------------------

	void VulkanDescriptorSetBuilder::AddBinding(const VkDescriptorType& type, const uint32_t& binding, const uint32_t& count)
	{
		VkDescriptorSetLayoutBinding newBind{};
		newBind.descriptorType = type;
		newBind.binding = binding;
		newBind.descriptorCount = count;
		newBind.pImmutableSamplers = nullptr;

		m_Bindings.emplace_back(newBind);
	}

	void VulkanDescriptorSetBuilder::Clear()
	{
		m_Bindings.clear();
	}

	VkDescriptorSetLayout VulkanDescriptorSetBuilder::Build(const VkDevice& logicalDevice, const VkShaderStageFlags& stageFlags, void* pNext, const VkDescriptorSetLayoutCreateFlags& createFlags)
	{
		for (auto& binding : m_Bindings)
			binding.stageFlags |= stageFlags;

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.pNext = pNext;

		createInfo.pBindings = m_Bindings.data();
		createInfo.bindingCount = static_cast<uint32_t>(m_Bindings.size());
		createInfo.flags = createFlags;

		VkDescriptorSetLayout layout;
		if (vkCreateDescriptorSetLayout(logicalDevice, &createInfo, nullptr, &layout) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to build Vulkan Descriptor Set Layout!");
			return VK_NULL_HANDLE;
		}

		return layout;
	}

	// -----------------------
	// SET MODIFIER ----------
	// -----------------------

	void VulkanDescriptorSetModifier::UpdateBufferBinding(const VulkanDescriptorUpdateInfo& updateInfo, VkDescriptorBufferInfo* bufferInfos)
	{
		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = nullptr;
		write.dstBinding = updateInfo.binding;
		write.dstSet = updateInfo.set;
		write.descriptorCount = updateInfo.count; // # of descriptors to update
		write.pBufferInfo = bufferInfos;
		write.descriptorType = updateInfo.type;
		write.dstArrayElement = updateInfo.elementOffset;

		m_Writes.push_back(write);
	}

	void VulkanDescriptorSetModifier::UpdateImageBinding(const VulkanDescriptorUpdateInfo& updateInfo, VkDescriptorImageInfo* imageInfos)
	{
		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = nullptr;
		write.dstBinding = updateInfo.binding;
		write.dstSet = updateInfo.set;
		write.descriptorCount = updateInfo.count; // # of descriptors to update
		write.pImageInfo = imageInfos;
		write.descriptorType = updateInfo.type;
		write.dstArrayElement = updateInfo.elementOffset;

		m_Writes.push_back(write);
	}

	void VulkanDescriptorSetModifier::ModifySets(const VkDevice& logicalDevice)
	{
		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(m_Writes.size()), m_Writes.data(), 0, nullptr);
		m_Writes.clear();
	}

	// ------------------------
	// SET ALLOCATOR ----------
	// ------------------------

	VulkanDescriptorSetAllocator::VulkanDescriptorSetAllocator(const VkDescriptorPool& pool)
		: m_Pool(pool) {}

	VkDescriptorSet VulkanDescriptorSetAllocator::Allocate(const VkDevice& logicalDevice, const VkDescriptorSetLayout& layout)
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.descriptorPool = m_Pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkDescriptorSet set;
		if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, &set) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create descriptor set");
		}

		return set;
	}

	void VulkanDescriptorSetAllocator::ClearDescriptors(const VkDevice& logicalDevice)
	{
		vkResetDescriptorPool(logicalDevice, m_Pool, 0);
	}
}
