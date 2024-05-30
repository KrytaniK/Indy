#include <Engine/Core/LogMacros.h>

#include <span>
#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanDescriptorPool::VulkanDescriptorPool(const VkDevice& logicalDevice, const uint32_t& maxSets, std::span<Ratio> ratios)
		: m_LogicalDevice(logicalDevice)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		for (Ratio& ratio : ratios)
			poolSizes.emplace_back(ratio.type, static_cast<uint32_t>(ratio.ratio) * maxSets);

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

		VkDescriptorSet descSet;
		if (vkAllocateDescriptorSets(m_LogicalDevice, &allocInfo, &descSet) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create descriptor set!");
			return {};
		}

		return descSet;
	}

	void VulkanDescriptorPool::ClearDescriptors()
	{
		vkResetDescriptorPool(m_LogicalDevice, m_DescriptorPool, 0);
	}

	// Layout Builder
	// -------------------------------------------------------------------

	void VulkanDescriptorLayoutBuilder::AddBinding(const VkDescriptorType& type, const uint32_t& binding, const uint32_t& count)
	{
		VkDescriptorSetLayoutBinding newBind{};
		newBind.descriptorType = type;
		newBind.binding = binding;
		newBind.descriptorCount = count;
		newBind.pImmutableSamplers = nullptr;

		bindings.emplace_back(newBind);
	}

	void VulkanDescriptorLayoutBuilder::Clear()
	{
		bindings.clear();
	}

	VkDescriptorSetLayout VulkanDescriptorLayoutBuilder::Build(const VkDevice& logicalDevice, const BuildInfo& buildInfo)
	{
		for (auto& binding : bindings)
			binding.stageFlags |= buildInfo.shaderStages;

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.pNext = buildInfo.pNext;

		createInfo.pBindings = bindings.data();
		createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		createInfo.flags = buildInfo.flags;

		VkDescriptorSetLayout layout;
		if (vkCreateDescriptorSetLayout(logicalDevice, &createInfo, nullptr, &layout) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to build Vulkan Descriptor Set Layout!");
			return VK_NULL_HANDLE;
		}

		return layout;
	}
}
