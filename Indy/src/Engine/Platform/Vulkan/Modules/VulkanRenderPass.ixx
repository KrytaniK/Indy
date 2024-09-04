module;

#include <cstdint>
#include <string>
#include <vector>

export module Indy.VulkanGraphics:RenderPass;

import Indy.Graphics;

export
{
	namespace Indy::Graphics
	{
		class VulkanRenderPass : public RenderPass
		{
		public:
			VulkanRenderPass(const std::string& alias, const uint32_t& id);
			virtual ~VulkanRenderPass() override;

			virtual const std::string& GetAlias() override { return m_Alias; };
			virtual const uint32_t& GetID() override { return m_ID; };

			virtual VulkanRenderPass& Begin() override;

			virtual RenderPass& AddProcess(const ProcessType& type) override;

			virtual RenderPass& BindShader(const PipelineShaderStage& shaderStage, const std::string& path) override;

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

			// Pipeline State
		};
	}
}