#include <Engine/Core/LogMacros.h>

#include <vector>
#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanDescriptorSetLayoutBuilder::VulkanDescriptorSetLayoutBuilder(const VkDevice& logicalDevice)
		: m_LogicalDevice(logicalDevice) {}

	void VulkanDescriptorSetLayoutBuilder::AddBinding(const uint32_t& binding, const VkDescriptorType& type)
	{
		VkDescriptorSetLayoutBinding newBind{};
		newBind.binding = binding;
		newBind.descriptorCount = 1;
		newBind.descriptorType = type;

		m_Bindings.emplace_back(newBind);
	}

	void VulkanDescriptorSetLayoutBuilder::Clear()
	{
		m_Bindings.clear();
	}

	VkDescriptorSetLayout VulkanDescriptorSetLayoutBuilder::Build(const VkShaderStageFlags& shaderStages, void* pNext,
		const VkDescriptorSetLayoutCreateFlags& flags)
	{
		for (auto& binding : m_Bindings)
			binding.stageFlags |= shaderStages;

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.pNext = pNext;

		createInfo.pBindings = m_Bindings.data();
		createInfo.bindingCount = static_cast<uint32_t>(m_Bindings.size());
		createInfo.flags = flags;

		VkDescriptorSetLayout layout;

		if (vkCreateDescriptorSetLayout(m_LogicalDevice, &createInfo, nullptr, &layout) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to build Vulkan Descriptor Set Layout!");
			return VK_NULL_HANDLE;
		}

		return layout;
	}
}
