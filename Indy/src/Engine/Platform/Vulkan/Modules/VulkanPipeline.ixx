module;

#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Pipeline;

import :DescriptorPool;
import :Descriptor;

import Indy.Graphics;

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

		class VulkanPipeline : public Pipeline
		{
		public:
			static VkShaderStageFlagBits GetShaderStage(const ShaderType& shaderType);

		public:
			VulkanPipeline(const VkDevice& logicalDevice, const VulkanPipelineInfo& info)
				: m_Info(info), m_LogicalDevice(logicalDevice) {};

			virtual ~VulkanPipeline() override;

			// Resource fetching
			const VkPipeline& Get() const { return m_Info.pipeline; };
			const VulkanPipelineInfo& GetInfo() const { return m_Info; };
			const VkPipelineLayout& GetLayout() const { return m_Info.layout; };
			virtual const PipelineType& GetType() const override { return m_Info.type; };
			VulkanDescriptor* GetDescriptor(const ShaderType& shaderType);

			// Pipeline Build process functions
			void BindShader(Shader& shader) override;
			void BindDescriptorSetLayout(const ShaderType& shaderType, VulkanDescriptorPool* descriptorPool, const VkDescriptorSetLayout& layout);

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
			std::unordered_map<ShaderType, VulkanDescriptor> m_Descriptors;
			std::unordered_map<ShaderType, VkShaderModule> m_ShaderModules;
		};
	}
}