module;

#include <vector>
#include <memory>
#include <cstdint>

#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Pipeline;

import :Descriptor;
import :Device;

import Indy.Graphics;

export
{
	namespace Indy
	{
		struct VulkanShader : Shader
		{
			std::vector<uint32_t> spv;
		};

		struct VulkanPipeline
		{
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout layout = VK_NULL_HANDLE;
			VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
			VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		};

		struct VulkanPipelineBuildOptions : PipelineBuildOptions
		{

		};

		class VulkanPipelineBuilder : public PipelineBuilder
		{
		public:
			VulkanPipelineBuilder(const VkDevice& device);
			virtual ~VulkanPipelineBuilder() override;

			virtual void Build(PipelineBuildOptions* options) override;

			virtual void Clear() override;

			virtual void BindShader(const std::string& shaderPath) override;
			virtual void BindShader(const Shader& shader, const PipelineShaderStage& stage) override;

			VulkanPipeline GetPipeline() { return m_Pipeline; };

		private:
			void BuildComputePipeline();
			void BuildGraphicsPipeline(VulkanPipelineBuildOptions* options);

		private:
			VkDevice m_LogicalDevice; // GPU Interface
			VulkanPipeline m_Pipeline; // Finalized Pipeline

			std::vector<VulkanShader> m_Shaders; // Bound Shaders
			std::vector<VkShaderModule> m_ShaderModules; // Created Shader Modules
		};

		VkShaderStageFlagBits GetVulkanShaderStage(const ShaderType& type);

		shaderc_shader_kind GetShadercType(const ShaderType& type);

		std::vector<uint32_t> CompileSPIRVFromGLSL(const VulkanShader& shader);

		void ReflectVulkanShader(
			const VulkanShader& shader,
			VulkanDescriptorSetBuilder& layoutBuilder,
			std::vector<VkDescriptorPoolSize>& poolSizes,
			std::vector<VkPushConstantRange>& pcRanges
		);
	}
}