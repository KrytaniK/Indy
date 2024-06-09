module;

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Pipeline;

import :DescriptorPool;
import :Descriptor;

import Indy.Graphics;

// TODO: Separate into Pipeline Builder and Pipeline

export
{
	namespace Indy
	{
		struct VulkanPipelineInfo
		{
			PipelineType type = INDY_PIPELINE_TYPE_GRAPHICS;
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout layout = VK_NULL_HANDLE;
		};

		class VulkanPipelineBuilder
		{

		};

		class VulkanPipeline : public Pipeline
		{
		public:
			static VkShaderStageFlagBits GetShaderStage(const ShaderType& shaderType);

		public:
			VulkanPipeline(const VkDevice& logicalDevice, const VulkanPipelineInfo& info);

			virtual ~VulkanPipeline() override;

			// Resource fetching
			const VkPipeline& Get() const { return m_Info.pipeline; };
			const VulkanPipelineInfo& GetInfo() const { return m_Info; };
			const VkPipelineLayout& GetLayout() const { return m_Info.layout; };
			virtual const PipelineType& GetType() const override { return m_Info.type; };
			const std::shared_ptr<VulkanDescriptor>& GetDescriptor(const ShaderType& shaderType);

			// Pipeline Build process functions
			void BindShader(Shader& shader) override;
			void BindDescriptorSetLayout(const ShaderType& shaderType, const VulkanDescriptorPool& descriptorPool, const VkDescriptorSetLayout& layout);

			// Inherited Build function
			virtual void Build() override;

		private:
			// Internal build functions
			void BuildComputePipeline();
			void BuildGraphicsPipeline();
			void BuildRayTracePipeline();

		private:
			VulkanPipelineInfo m_Info;
			VkDevice m_LogicalDevice;
			std::vector<std::shared_ptr<VulkanDescriptor>> m_Descriptors;
			std::vector<VkShaderModule> m_ShaderModules;
		};
	}
}