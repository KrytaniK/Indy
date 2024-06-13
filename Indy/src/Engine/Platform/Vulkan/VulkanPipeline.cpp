#include <Engine/Core/LogMacros.h>

#include <any>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VkShaderStageFlagBits VulkanPipeline::GetShaderStage(const ShaderType& shaderType)
	{
		switch(shaderType)
		{
			case INDY_SHADER_TYPE_COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
			case INDY_SHADER_TYPE_VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
			case INDY_SHADER_TYPE_FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case INDY_SHADER_TYPE_TESS_CONTROL: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case INDY_SHADER_TYPE_TESS_EVAL: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			default: return VK_SHADER_STAGE_ALL;
		}
	}

	VulkanPipeline::VulkanPipeline(const VkDevice& logicalDevice, const VulkanPipelineInfo& info)
		: m_Info(info), m_LogicalDevice(logicalDevice)
	{
		// Resize to max value of shader type enum
		m_Descriptors.resize(6); 
		m_PushConstants.resize(6);
		m_ShaderModules.resize(6); 
	}

	VulkanPipeline::~VulkanPipeline()
	{
		for (const auto& shaderModule : m_ShaderModules)
			vkDestroyShaderModule(m_LogicalDevice, shaderModule, nullptr);

		for (auto& descriptor : m_Descriptors)
			vkDestroyDescriptorSetLayout(m_LogicalDevice, descriptor->GetLayout(), nullptr);

		vkDestroyPipelineLayout(m_LogicalDevice, m_Info.layout, nullptr);
		vkDestroyPipeline(m_LogicalDevice, m_Info.pipeline, nullptr);
	}

	const std::shared_ptr<VulkanDescriptor>& VulkanPipeline::GetDescriptor(const ShaderType& shaderType)
	{
		if (shaderType >= m_Descriptors.size())
			return nullptr;

		return m_Descriptors[shaderType];
	}

	void VulkanPipeline::BindShader(Shader& shader)
	{
		// Get or compile SPIR-V shader
		auto spv = shader.GetSPIRV();
		if (spv.size == 0)
		{
			shader.CompileSPIRV();
			spv = shader.GetSPIRV();
		}

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spv.size;
		createInfo.pCode = spv.data;

		if (vkCreateShaderModule(m_LogicalDevice, &createInfo, nullptr, &m_ShaderModules[shader.GetType()]) != VK_SUCCESS)
			INDY_CORE_ERROR("failed to create shader module!");
	}

	void VulkanPipeline::BindDescriptorSetLayout(const ShaderType& shaderType, const VulkanDescriptorPool& descriptorPool, const VkDescriptorSetLayout& layout)
	{
		m_Descriptors[shaderType] = std::make_shared<VulkanDescriptor>(descriptorPool, layout);
	}

	void VulkanPipeline::BindPushConstants(const ShaderType& shaderType, const VkPushConstantRange& pushConstantRange)
	{
		m_PushConstants[shaderType] = std::make_shared<VkPushConstantRange>(pushConstantRange);
	}

	void VulkanPipeline::Build()
	{
		switch(m_Info.type)
		{
			case INDY_PIPELINE_TYPE_COMPUTE: { BuildComputePipeline(); return; }
			case INDY_PIPELINE_TYPE_RAYTRACE: { BuildRayTracePipeline(); return; }
			case INDY_PIPELINE_TYPE_GRAPHICS: { BuildGraphicsPipeline(); return; }
			default:
			{
				INDY_CORE_ERROR("Invalid Pipeline Type");
			}
		}
	}

	void VulkanPipeline::BuildComputePipeline() 
	{
		// Get all layouts
		// -------------------------------------------------------------------------------------------------------

		std::vector<VkDescriptorSetLayout> layouts;
		for (std::shared_ptr<VulkanDescriptor>& descriptor : m_Descriptors)
			if (descriptor)
				layouts.emplace_back(descriptor->GetLayout());

		// Get all push constant ranges
		// -------------------------------------------------------------------------------------------------------
		std::vector<VkPushConstantRange> pushConstants;
		for (std::shared_ptr<VkPushConstantRange>& pushConstant : m_PushConstants)
			if (pushConstant)
				pushConstants.emplace_back(*pushConstant);
		

		// Build pipeline layout
		// -------------------------------------------------------------------------------------------------------

		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.pSetLayouts = layouts.data();
		createInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
		createInfo.pPushConstantRanges = pushConstants.data();
		createInfo.pushConstantRangeCount = pushConstants.size();

		if (vkCreatePipelineLayout(m_LogicalDevice, &createInfo, nullptr, &m_Info.layout) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Could not create compute pipeline layout!");
			return;
		}

		// Build Pipeline
		// -------------------------------------------------------------------------------------------------------

		VkPipelineShaderStageCreateInfo stageInfo{};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.pNext = nullptr;
		stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageInfo.module = m_ShaderModules[INDY_SHADER_TYPE_COMPUTE];
		stageInfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.layout = m_Info.layout;
		computePipelineCreateInfo.stage = stageInfo;

		if (vkCreateComputePipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_Info.pipeline) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create compute pipeline!");
		}
	}

	void VulkanPipeline::BuildGraphicsPipeline()
	{
		// Get all layouts
		// -------------------------------------------------------------------------------------------------------

		std::vector<VkDescriptorSetLayout> layouts;
		for (std::shared_ptr<VulkanDescriptor>& descriptor : m_Descriptors)
			if (descriptor)
				layouts.emplace_back(descriptor->GetLayout());

		// Build pipeline layout
		// -------------------------------------------------------------------------------------------------------

		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.pSetLayouts = layouts.data();
		createInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());

		if (vkCreatePipelineLayout(m_LogicalDevice, &createInfo, nullptr, &m_Info.layout) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Could not create compute pipeline layout!");
			return;
		}

		// Build Pipeline
		// -------------------------------------------------------------------------------------------------------

		// TODO: Implement Graphics Pipeline!
	}

	void VulkanPipeline::BuildRayTracePipeline()
	{
		// Get all layouts
		// -------------------------------------------------------------------------------------------------------

		std::vector<VkDescriptorSetLayout> layouts;
		for (std::shared_ptr<VulkanDescriptor>& descriptor : m_Descriptors)
			if (descriptor)
				layouts.emplace_back(descriptor->GetLayout());

		// Build pipeline layout
		// -------------------------------------------------------------------------------------------------------

		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.pSetLayouts = layouts.data();
		createInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());

		if (vkCreatePipelineLayout(m_LogicalDevice, &createInfo, nullptr, &m_Info.layout) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Could not create compute pipeline layout!");
			return;
		}

		// Build Pipeline
		// -------------------------------------------------------------------------------------------------------

		// TODO: Implement Ray Tracing Pipeline!
	}
}