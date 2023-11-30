#include "DescriptorPool.h"

namespace Engine::VulkanAPI
{
	VkDescriptorPool DescriptorPool::s_DescriptorPool;
	std::vector<VkDescriptorSetLayout> DescriptorPool::s_DescriptorSetLayouts;

	void DescriptorPool::Init(const VkDevice& logicalDevice, const uint32_t& descriptorCount)
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = descriptorCount;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = descriptorCount;

		if (vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &s_DescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void DescriptorPool::Shutdown(const VkDevice& logicalDevice)
	{
		for (auto layout : s_DescriptorSetLayouts)
		{
			vkDestroyDescriptorSetLayout(logicalDevice, layout, nullptr);
		}

		vkDestroyDescriptorPool(logicalDevice, s_DescriptorPool, nullptr);
	}

	void DescriptorPool::AllocateDescriptorSets(const VkDevice& logicalDevice, Viewport& viewport, VkDescriptorSetLayout descriptorSetLayout)
	{
		const uint32_t descriptorSetCount = static_cast<uint32_t>(viewport.frames.size());

		std::vector<VkDescriptorSetLayout> layouts(descriptorSetCount, descriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = s_DescriptorPool;
		allocInfo.descriptorSetCount = descriptorSetCount;
		allocInfo.pSetLayouts = layouts.data();

		viewport.descriptorSets.resize(descriptorSetCount);
		if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, viewport.descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
	}

	void DescriptorPool::UpdateDescriptorSets(const VkDevice& logicalDevice, const Viewport& viewport, const VkDeviceSize& range, const VkDescriptorType& descriptorType)
	{
		const uint32_t descriptorSetCount = (uint32_t)viewport.frames.size();

		for (size_t i = 0; i < descriptorSetCount; i++)
		{
			VkBuffer buffer = viewport.frames[i].uniformBuffer->GetBuffer();

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = range;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = viewport.descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = descriptorType;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, nullptr);
		}
	}

	void DescriptorPool::CreateUBODescriptorSetLayout(const VkDevice& logicalDevice)
	{

		VkDescriptorSetLayoutBinding uboLayoutBinding = 
			CreateSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

		size_t index = s_DescriptorSetLayouts.size();
		s_DescriptorSetLayouts.resize(s_DescriptorSetLayouts.size() + 1);
		CreateSetLayout(logicalDevice, s_DescriptorSetLayouts[index], uboLayoutBinding);
	}

	VkDescriptorSetLayoutBinding DescriptorPool::CreateSetLayoutBinding(uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stageFlags, VkSampler* immutableSamplers)
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = type;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		layoutBinding.pImmutableSamplers = immutableSamplers; // Optional

		return layoutBinding;
	}

	void DescriptorPool::CreateSetLayout(const VkDevice& logicalDevice, VkDescriptorSetLayout& outLayout, VkDescriptorSetLayoutBinding layoutBinding)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &layoutBinding;

		if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &outLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}
}