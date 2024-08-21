module;

#include <cstdint>
#include <string>
#include <vector>

export module Indy.Graphics:RenderPass;

import :Types;

export
{
	namespace Indy::Graphics
	{
		class RenderPass
		{
		public:
			virtual ~RenderPass() = default;

			virtual const uint32_t& GetID() = 0;

			virtual const RenderPass& Begin(const std::string& alias) = 0;
			virtual const RenderPass& Begin(const uint32_t& id) = 0;

			virtual void End() = 0;

			virtual const RenderPass& AddPreProcessShader(const std::string& shaderPath) = 0;

			virtual const RenderPass& BindVertexShader(const std::string& shaderPath) = 0;

			virtual const RenderPass& BindTessellationControlShader(const std::string& shaderPath) = 0;

			virtual const RenderPass& BindTessellationEvalShader(const std::string& shaderPath) = 0;

			virtual const RenderPass& BindGeometryShader(const std::string& shaderPath) = 0;

			virtual const RenderPass& BindFragmentShader(const std::string& shaderPath) = 0;

			virtual const RenderPass& AddPostProcessShader(const std::string& shaderPath) = 0;

			virtual const RenderPass& SetTopology(const PrimitiveTopology& topology) = 0;

			virtual const RenderPass& SetPolygonMode(const PolygonMode& mode) = 0;

			virtual const RenderPass& SetFrontFace(const FrontFace& frontFace) = 0;

			virtual const RenderPass& SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks = {}) = 0;

			virtual const RenderPass& EnableDepthTesting(const bool enabled = true) = 0;

			virtual const RenderPass& EnableDepthWriting(const bool enabled = false) = 0;
		};
	}
}