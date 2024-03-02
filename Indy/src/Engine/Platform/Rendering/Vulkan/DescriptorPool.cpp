#include "DescriptorPool.h"

namespace Engine::VulkanAPI
{
	VkDescriptorPool DescriptorPool::s_DescriptorPool;
	std::vector<VkDescriptorSetLayout> DescriptorPool::s_DescriptorSetLayouts;

	void DescriptorPool::Init(const VkDevice& logicalDevice, const uint32_t& descriptorCount)
	{
		VkDescriptorPoolSize ubo_PoolSize{};
		ubo_PoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		ubo_PoolSize.descriptorCount = descriptorCount;

		std::vector<VkDescriptorPoolSize> poolSizes = {
			ubo_PoolSize
		};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
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
		for (size_t i = 0; i < viewport.frames.size(); i++)
		{
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = s_DescriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &descriptorSetLayout;

			if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, &viewport.frames[i].descriptorSet) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate descriptor sets!");
			}
		}
	}

	void DescriptorPool::UpdateDescriptorSets(const VkDevice& logicalDevice, const Viewport& viewport)
	{
		std::vector<VkWriteDescriptorSet> writeDescriptorSets;

		for (size_t i = 0; i < viewport.frames.size(); i++)
		{
			VkBuffer viewBuffer = viewport.frames[i].uniformBuffers.view.buffer->Get();

			VkDescriptorBufferInfo viewBufferInfo{};
			viewBufferInfo.buffer = viewBuffer;
			viewBufferInfo.offset = 0;
			viewBufferInfo.range = sizeof(ViewProjectionMatrix);

			VkWriteDescriptorSet viewBufferDescriptorWrite{};
			viewBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			viewBufferDescriptorWrite.dstSet = viewport.frames[i].descriptorSet;
			viewBufferDescriptorWrite.dstBinding = 0;
			viewBufferDescriptorWrite.dstArrayElement = 0;
			viewBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			viewBufferDescriptorWrite.descriptorCount = 1;
			viewBufferDescriptorWrite.pBufferInfo = &viewBufferInfo;
			viewBufferDescriptorWrite.pImageInfo = nullptr; // Optional
			viewBufferDescriptorWrite.pTexelBufferView = nullptr; // Optional

			writeDescriptorSets.emplace_back(viewBufferDescriptorWrite);
		}

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}

	void DescriptorPool::CreateUBODescriptorSetLayout(const VkDevice& logicalDevice)
	{
		std::vector<VkDescriptorSetLayoutBinding> descriptorLayouts = {
			CreateSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT)
		};

		size_t index = s_DescriptorSetLayouts.size();
		s_DescriptorSetLayouts.resize(s_DescriptorSetLayouts.size() + 1);
		CreateSetLayout(logicalDevice, descriptorLayouts.data(), static_cast<uint32_t>(descriptorLayouts.size()), s_DescriptorSetLayouts[index]);
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

	void DescriptorPool::CreateSetLayout(const VkDevice& logicalDevice, VkDescriptorSetLayoutBinding* layoutBindings, uint32_t bindingCount, VkDescriptorSetLayout& outLayout)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindingCount;
		layoutInfo.pBindings = layoutBindings;

		if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &outLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}
}