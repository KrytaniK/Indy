#include <Engine/Core/LogMacros.h>

#include <any>

#include <vulkan/vulkan.h>

import Indy.VulkanGraphics;

namespace Indy
{
	VulkanPipeline::VulkanPipeline(const PipelineType& type, const VkDevice& logicalDevice)
		: m_Type(type), m_LogicalDevice(logicalDevice), m_Pipeline(VK_NULL_HANDLE), m_PipelineLayout(VK_NULL_HANDLE)
	{
	}

	VulkanPipeline::~VulkanPipeline()
	{
		for (const auto& shaderModule : m_ShaderModules)
			vkDestroyShaderModule(m_LogicalDevice, shaderModule.second, nullptr);
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
		// TODO: Implement Compute Pipeline!
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