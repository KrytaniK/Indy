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

			virtual void End() override;

			virtual VulkanRenderPass& AddPreProcessShader(const std::string& shaderPath) override;

			virtual VulkanRenderPass& BindVertexShader(const std::string& shaderPath) override;

			virtual VulkanRenderPass& BindTessellationControlShader(const std::string& shaderPath) override;

			virtual VulkanRenderPass& BindTessellationEvalShader(const std::string& shaderPath) override;

			virtual VulkanRenderPass& BindGeometryShader(const std::string& shaderPath) override;

			virtual VulkanRenderPass& BindFragmentShader(const std::string& shaderPath) override;

			virtual VulkanRenderPass& AddPostProcessShader(const std::string& shaderPath) override;

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