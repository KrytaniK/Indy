module;

#include <unordered_map>
#include <memory>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Pipeline;

import Indy.Graphics;

export
{
	namespace Indy
	{
		class VulkanPipeline : public Pipeline
		{
		public:
			VulkanPipeline(const PipelineType& type, const VkDevice& logicalDevice);
			virtual ~VulkanPipeline() override;

			void BindShader(const PipelineShaderStage& stage, Shader& shader) override;

			void Build() override;

		private:
			void BuildComputePipeline();
			void BuildGraphicsPipeline();
			void BuildRayTracePipeline();

			VkPipelineShaderStageCreateInfo GenerateShaderStageInfo(const PipelineShaderStage& stage);

		private:
			PipelineType m_Type;
			VkDevice m_LogicalDevice;
			VkPipeline m_Pipeline;
			VkPipelineLayout m_PipelineLayout;
			std::unordered_map<PipelineShaderStage, VkShaderModule> m_ShaderModules;
		};
	}
}