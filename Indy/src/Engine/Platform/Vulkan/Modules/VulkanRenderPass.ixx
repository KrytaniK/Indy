module;

#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:RenderPass;

import Indy.Graphics;

export
{
	namespace Indy::Graphics
	{
		struct VulkanPipeline
		{
			VkPipeline handle;
			VkPipelineLayout layout;
			VkDescriptorSetLayout descriptorSetLayout;
			VkDescriptorPool descriptorPool;
		};

		class VulkanRenderPass : public RenderPass
		{
		private:
			struct Process
			{
				ProcessType type;
				PipelineType pipelineType;
				PipelineState pipelineState{};
			};

		public:
			VulkanRenderPass(const std::string& alias, const uint32_t& id);
			virtual ~VulkanRenderPass() override;

			virtual const std::string& GetAlias() override { return m_Alias; };
			virtual const uint32_t& GetID() override { return m_ID; };

			virtual VulkanRenderPass& Begin() override;

			virtual VulkanRenderPass& AddProcess(const ProcessType& type) override;

			virtual VulkanRenderPass& BindShader(const PipelineShaderStage& shaderStage, const std::string& path) override;

			virtual void End() override;

			virtual VulkanRenderPass& SetTopology(const PrimitiveTopology& topology) override;

			virtual VulkanRenderPass& SetPolygonMode(const PolygonMode& mode) override;

			virtual VulkanRenderPass& SetFrontFace(const FrontFace& frontFace) override;

			virtual VulkanRenderPass& SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks = {}) override;

			virtual VulkanRenderPass& EnableDepthTesting(const bool enabled = false) override;

			virtual VulkanRenderPass& EnableDepthWriting(const bool enabled = false) override;

		private:
			std::string m_Alias;
			uint32_t m_ID;

			bool m_IsConfigurable;

			std::vector<PipelineState> m_PreProcessState;
			PipelineState m_MainProcessState;
			std::vector<PipelineState> m_PostProcessState;

			std::vector<VulkanPipeline> m_Pipelines;
		};
	}
}