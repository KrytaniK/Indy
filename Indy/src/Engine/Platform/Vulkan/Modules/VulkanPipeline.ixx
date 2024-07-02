module;

#include <vector>
#include <memory>
#include <cstdint>

#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Pipeline;

import :DescriptorPool;
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

			std::shared_ptr<VulkanDescriptorPool> descriptorPool;
			std::vector<VulkanDescriptor> descriptors;
		};

		class VulkanPipelineBuilder : public PipelineBuilder
		{
		public:
			VulkanPipelineBuilder(const VkDevice& device);
			virtual ~VulkanPipelineBuilder() override = default;

			virtual void Build(PipelineBuildOptions* options = nullptr) override;

			virtual void Clear() override;

			virtual void BindShader(const std::string& shaderPath) override;
			virtual void BindShader(const Shader& shader, const PipelineShaderStage& stage) override;

			VulkanPipeline GetPipeline() { return m_Pipeline; };

		private:
			void BuildComputePipeline();

		private:
			VkDevice m_LogicalDevice;
			VulkanPipeline m_Pipeline;
			std::vector<VulkanShader> m_Shaders;
			std::vector<VkShaderModule> m_ShaderModules;
		};

		shaderc_shader_kind GetShadercType(const ShaderType& type);

		std::vector<uint32_t> CompileSPIRVFromGLSL(const VulkanShader& shader);



		//struct VulkanPipelineInfo
		//{
		//	PipelineType type = INDY_PIPELINE_TYPE_GRAPHICS;
		//	VkPipeline pipeline = VK_NULL_HANDLE;
		//	VkPipelineLayout layout = VK_NULL_HANDLE;
		//};

		//class VulkanPipeline : public Pipeline
		//{
		//public:
		//	static VkShaderStageFlagBits GetShaderStage(const ShaderType& shaderType);

		//public:
		//	VulkanPipeline(const VkDevice& logicalDevice, const VulkanPipelineInfo& info);

		//	virtual ~VulkanPipeline() override;

		//	// Resource fetching
		//	const VkPipeline& Get() const { return m_Info.pipeline; };
		//	const VulkanPipelineInfo& GetInfo() const { return m_Info; };
		//	const VkPipelineLayout& GetLayout() const { return m_Info.layout; };
		//	virtual const PipelineType& GetType() const override { return m_Info.type; };
		//	const std::shared_ptr<VulkanDescriptor>& GetDescriptor(const ShaderType& shaderType);

		//	// Pipeline Build process functions
		//	void BindShader(Shader& shader) override;
		//	void BindDescriptorSetLayout(const ShaderType& shaderType, const VulkanDescriptorPool& descriptorPool, const VkDescriptorSetLayout& layout);
		//	void BindPushConstants(const ShaderType& shaderType, const VkPushConstantRange& pushConstantRange);

		//	// Inherited Build function
		//	virtual void Build() override;

		//private:
		//	// Internal build functions
		//	void BuildComputePipeline();
		//	void BuildGraphicsPipeline();
		//	void BuildRayTracePipeline();

		//private:
		//	VulkanPipelineInfo m_Info;
		//	VkDevice m_LogicalDevice;
		//	std::vector<VkShaderModule> m_ShaderModules;
		//	std::vector<std::shared_ptr<VulkanDescriptor>> m_Descriptors;
		//	std::vector<std::shared_ptr<VkPushConstantRange>> m_PushConstants;
		//};
	}
}