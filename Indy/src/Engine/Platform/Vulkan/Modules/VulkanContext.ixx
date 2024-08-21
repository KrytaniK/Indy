module;

#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Context;

import Indy.Graphics;

export
{
	namespace Indy::Graphics
	{
		class VulkanContext : public RenderContext
		{
		public:
			VulkanContext(const VkInstance& instance, const uint32_t& id, const std::string& alias);
			VulkanContext(RenderContext* context, const VkInstance& instance);
			virtual ~VulkanContext() override;

			virtual const std::string& GetAlias() const override { return m_Alias; };

			virtual uint32_t GetID() const override { return m_ID; };

			virtual bool AddRenderPass(const RenderPass& renderPass) const override;

			virtual const RenderContext& BeginRenderPass(const std::string& debugName) const override;
			virtual const RenderContext& BeginRenderPass(const uint32_t& id) const override;

			virtual void EndRenderPass() const override;

			virtual const RenderPass& GetRenderPass(const uint32_t& id) const override;

			virtual const RenderContext& AddPreProcessShader(const std::string& shaderPath) const override;

			virtual const RenderContext& BindVertexShader(const std::string& shaderPath) const override;

			virtual const RenderContext& BindTessellationControlShader(const std::string& shaderPath) const override;

			virtual const RenderContext& BindTessellationEvalShader(const std::string& shaderPath) const override;

			virtual const RenderContext& BindGeometryShader(const std::string& shaderPath) const override;

			virtual const RenderContext& BindFragmentShader(const std::string& shaderPath) const override;

			virtual const RenderContext& AddPostProcessShader(const std::string& shaderPath) const override;

			virtual const RenderContext& SetTopology(const PrimitiveTopology& topology) const override;

			virtual const RenderContext& SetPolygonMode(const PolygonMode& mode) const override;

			virtual const RenderContext& SetFrontFace(const FrontFace& frontFace) const override;

			virtual const RenderContext& SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks = {}) const override;

			virtual const RenderContext& EnableDepthTesting(const bool enabled = true) const override;

			virtual const RenderContext& EnableDepthWriting(const bool enabled = false) const override;

			virtual bool SetActiveViewport(const uint32_t& id) const override;
			virtual bool SetActiveViewport(const std::string& alias) const override;

		private:
			uint32_t m_ID;
			std::string m_Alias;
			VkInstance m_Instance;
			// Vulkan Device

			std::vector<Viewport> m_Viewports;
			std::vector<RenderPass> m_RenderPasses;
		};
	}
}