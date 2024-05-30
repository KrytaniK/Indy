#include <Engine/Core/LogMacros.h>

#include <any>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanPipeline::VulkanPipeline(const VkDevice& logicalDevice, const PipelineType& type)
		: m_LogicalDevice(logicalDevice), m_Type(type), m_Pipeline(VK_NULL_HANDLE), m_PipelineLayout(VK_NULL_HANDLE)
	{ }

	VulkanPipeline::~VulkanPipeline()
	{
		for (const auto& shaderModule : m_ShaderModules)
			vkDestroyShaderModule(m_LogicalDevice, shaderModule.second, nullptr);

		vkDestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout, nullptr);
		vkDestroyPipeline(m_LogicalDevice, m_Pipeline, nullptr);
	}

	void VulkanPipeline::BindShader(const PipelineShaderStage& stage, Shader& shader)
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

		if (vkCreateShaderModule(m_LogicalDevice, &createInfo, nullptr, &m_ShaderModules[stage]) != VK_SUCCESS)
			INDY_CORE_ERROR("failed to create shader module!");
	}

	void VulkanPipeline::AddDescriptorSetLayout(const VkDescriptorSetLayout& layout)
	{
		m_DescSetLayouts.emplace_back(layout);
	}

	void VulkanPipeline::Build()
	{
		switch(m_Type)
		{
			case INDY_PIPELINE_TYPE_COMPUTE: { BuildComputePipeline(); return; }
			case INDY_PIPELINE_TYPE_RAY_TRACING: { BuildRayTracePipeline(); return; }
			default: { BuildGraphicsPipeline(); }
		}
	}

	void VulkanPipeline::BuildComputePipeline()
	{
		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.pSetLayouts = m_DescSetLayouts.data();
		createInfo.setLayoutCount = (uint32_t)m_DescSetLayouts.size();

		if (vkCreatePipelineLayout(m_LogicalDevice, &createInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Could not create compute pipeline layout!");
			return;
		}

		VkPipelineShaderStageCreateInfo stageInfo{};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.pNext = nullptr;
		stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageInfo.module = m_ShaderModules[INDY_PIPELINE_SHADER_STAGE_COMPUTE];
		stageInfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.layout = m_PipelineLayout;
		computePipelineCreateInfo.stage = stageInfo;

		if (vkCreateComputePipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create compute pipeline!");
		}
	}

	void VulkanPipeline::BuildGraphicsPipeline()
	{
		// Programmable pipeline stages
		VkPipelineShaderStageCreateInfo shaderStageInfos[] = {
			GenerateShaderStageInfo(INDY_PIPELINE_SHADER_STAGE_VERTEX),
			GenerateShaderStageInfo(INDY_PIPELINE_SHADER_STAGE_VERTEX),
			GenerateShaderStageInfo(INDY_PIPELINE_SHADER_STAGE_FRAGMENT),
		};


	}

	void VulkanPipeline::BuildRayTracePipeline()
	{
		// TODO: Implement Ray Tracing Pipeline!
	}

	VkPipelineShaderStageCreateInfo VulkanPipeline::GenerateShaderStageInfo(const PipelineShaderStage& stage)
	{
		VkPipelineShaderStageCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		createInfo.module = m_ShaderModules[stage];
		createInfo.pName = "main"; // shader entrypoint

		switch(stage)
		{
		case INDY_PIPELINE_SHADER_STAGE_COMPUTE:
			{
				createInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;

				break;
			};
		case INDY_PIPELINE_SHADER_STAGE_VERTEX:
			{
				createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

				break;
			};
		case INDY_PIPELINE_SHADER_STAGE_FRAGMENT:
			{
				createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

				break;
			};
		case INDY_PIPELINE_SHADER_STAGE_TESS_CONTROL:
			{
				createInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

				break;
			};
		case INDY_PIPELINE_SHADER_STAGE_TESS_EVAL:
			{
				createInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

				break;
			};
		}

		return createInfo;
	};

}