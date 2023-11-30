#pragma once

#include "Util.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Engine::VulkanAPI
{
	class DescriptorPool
	{
	private:
		static VkDescriptorPool s_DescriptorPool;
		static std::vector<VkDescriptorSetLayout> s_DescriptorSetLayouts;

	public:
		static void Init(const VkDevice& logicalDevice, const uint32_t& descriptorCount);
		static void Shutdown(const VkDevice& logicalDevice);

		static void CreateUBODescriptorSetLayout(const VkDevice& logicalDevice);

		static void AllocateDescriptorSets(const VkDevice& logicalDevice, Viewport& viewport, VkDescriptorSetLayout descriptorSetLayout);
		static void UpdateDescriptorSets(const VkDevice& logicalDevice, const Viewport& viewport, const VkDeviceSize& range, const VkDescriptorType& descriptorType);

		static VkDescriptorSetLayout* GetDescriptorSetLayout(const uint32_t& layoutIndex) { return &s_DescriptorSetLayouts[layoutIndex]; }; // This is temp. A better solution is needed. Currently used in conjunction with the UBO_DESCRIPTOR_SET_LAYOUT macro

	private:
		static VkDescriptorSetLayoutBinding CreateSetLayoutBinding(uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stageFlags, VkSampler* immutableSamplers = nullptr);
		static void CreateSetLayout(const VkDevice& logicalDevice, VkDescriptorSetLayout& outLayout, VkDescriptorSetLayoutBinding layoutBinding);
	};

	#define UBO_DESCRIPTOR_SET_LAYOUT 0
}