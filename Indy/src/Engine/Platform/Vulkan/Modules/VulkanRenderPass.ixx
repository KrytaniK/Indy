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

			virtual const std::string& GetAlias() { return m_Alias; };
			virtual const uint32_t& GetID() { return m_ID; };

			virtual const VulkanRenderPass& Begin(const std::string& alias) override;
			virtual const VulkanRenderPass& Begin(const uint32_t& id) override;

			virtual void End() override;

			virtual const VulkanRenderPass& AddPreProcessShader(const std::string& shaderPath) override;

			virtual const VulkanRenderPass& BindVertexShader(const std::string& shaderPath) override;

			virtual const VulkanRenderPass& BindTessellationControlShader(const std::string& shaderPath) override;

			virtual const VulkanRenderPass& BindTessellationEvalShader(const std::string& shaderPath) override;

			virtual const VulkanRenderPass& BindGeometryShader(const std::string& shaderPath) override;

			virtual const VulkanRenderPass& BindFragmentShader(const std::string& shaderPath) override;

			virtual const VulkanRenderPass& AddPostProcessShader(const std::string& shaderPath) override;

			virtual const VulkanRenderPass& SetTopology(const PrimitiveTopology& topology) override;

			virtual const VulkanRenderPass& SetPolygonMode(const PolygonMode& mode) override;

			virtual const VulkanRenderPass& SetFrontFace(const FrontFace& frontFace) override;

			virtual const VulkanRenderPass& SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks = {}) override;

			virtual const VulkanRenderPass& EnableDepthTesting(const bool enabled = true) override;

			virtual const VulkanRenderPass& EnableDepthWriting(const bool enabled = false) override;

		private:
			std::string m_Alias;
			uint32_t m_ID;
		};
	}
}