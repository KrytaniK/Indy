module;

#include <unordered_map>
#include <vector>

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
			VulkanPipeline(const VkDevice& logicalDevice, const PipelineType& type);
			virtual ~VulkanPipeline() override;

			virtual const PipelineType& GetType() const override { return m_Type; };

			void BindShader(const PipelineShaderStage& stage, Shader& shader) override;

			void AddDescriptorSetLayout(const VkDescriptorSetLayout& layout);

			void Build() override;

			const VkPipeline& Get() { return m_Pipeline; };
			const VkPipelineLayout& GetLayout() { return m_PipelineLayout; };

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
			std::vector<VkDescriptorSetLayout> m_DescSetLayouts;

			std::unordered_map<PipelineShaderStage, VkShaderModule> m_ShaderModules;
		};
	}
}