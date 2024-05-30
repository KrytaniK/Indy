#include <Engine/Core/LogMacros.h>

#include <span>
#include <vector>
#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanDescriptorPool::VulkanDescriptorPool(const VkDevice& logicalDevice, const uint32_t& maxSets, std::span<Ratio> ratios)
		: m_LogicalDevice(logicalDevice)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		for (Ratio& ratio : ratios)
			poolSizes.emplace_back(ratio.type, ratio.ratio * maxSets);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = 0;
		poolInfo.maxSets = maxSets;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();

		vkCreateDescriptorPool(m_LogicalDevice, &poolInfo, nullptr, &m_DescriptorPool);
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		vkDestroyDescriptorPool(m_LogicalDevice, m_DescriptorPool, nullptr);
	}

	VkDescriptorSet VulkanDescriptorPool::AllocateDescriptorSet(const VkDescriptorSetLayout& layout)
	{
		VkDescriptorSetAllocateInfo allocInfo = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.pNext = nullptr;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkDescriptorSet descriptorSet;
		if (vkAllocateDescriptorSets(m_LogicalDevice, &allocInfo, &descriptorSet) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create descriptor set!");
			return VK_NULL_HANDLE;
		}

		return descriptorSet;
	}

	void VulkanDescriptorPool::ClearDescriptors()
	{
		vkResetDescriptorPool(m_LogicalDevice, m_DescriptorPool, 0);
	}

}